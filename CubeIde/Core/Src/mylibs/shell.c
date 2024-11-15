/*
 * shell.c
 *
 *  Created on: Oct 1, 2023
 *      Author: nicolas
 */
#include "usart.h"
#include "mylibs/shell.h"
#include "mylibs/pwm.h"
#include "mylibs/mesure_courant.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>


uint8_t prompt[]="lulu_chaha@Nucleo-STM32G474RET6>>";
uint8_t started[]=
		"\r\n*-----------------------------*"
		"\r\n| Welcome on Nucleo-STM32G474 |"
		"\r\n*-----------------------------*"
		"\r\n";
uint8_t newline[]="\r\n";
uint8_t backspace[]="\b \b";
uint8_t cmdNotFound[]="Command not found\r\n";

extern uint16_t adc_phase_values[PHASE_QUANTITY];
extern uint16_t adc_bus_values[BUS_QUANTITY];

struct h_shell_struct *h_shell;

char* 		argv[MAX_ARGS];
int		 	argc = 0;
char*		token;
int 		newCmdReady = 0;




int Shell_Add(h_shell_t *h_shell, char *name, shell_func_pointer_t pfunc, char *description)
{
	if (h_shell->shell_func_list_size < SHELL_FUNC_LIST_MAX_SIZE)
	{
		h_shell->shell_func_list[h_shell->shell_func_list_size].name = name;
		h_shell->shell_func_list[h_shell->shell_func_list_size].func = pfunc;
		h_shell->shell_func_list[h_shell->shell_func_list_size].description = description;
		h_shell->shell_func_list_size++;
		return 0;
	}
	return -1;
}

int Shell_Help(h_shell_t *h_shell,char **argv,int argc)
{
	int i;
	for (i = 0; i < h_shell->shell_func_list_size; i++)
	{
		int size;
		memset(h_shell->uartTxBuffer, 0, UART_TX_BUFFER_SIZE);
		size = snprintf((char *)h_shell->uartTxBuffer, UART_TX_BUFFER_SIZE, "%s: %s\r\n",
		                h_shell->shell_func_list[i].name,
		                h_shell->shell_func_list[i].description);

		HAL_UART_Transmit(&huart2, h_shell->uartTxBuffer,size, HAL_MAX_DELAY);

	}

	return 0;
}


void Shell_Init(void)
{
	memset(argv, 0, MAX_ARGS*sizeof(char*));
	memset(h_shell->cmdBuffer, 0, CMD_BUFFER_SIZE*sizeof(char));
	memset(h_shell->uartRxBuffer, 0, UART_RX_BUFFER_SIZE*sizeof(char));
	memset(h_shell->uartTxBuffer, 0, UART_TX_BUFFER_SIZE*sizeof(char));

	Shell_Add(h_shell, "help", Shell_Help, "Display help for known functions of the shell");
	Shell_Add(h_shell, "PWM_speed_control", PWM_Speed_Control, "Set the speed of the motor in %. 0 being Full reverse speed, 50 neutral and 100 full forward speed");
	Shell_Add(h_shell, "PWM_Start", PWM_Start, "Start PWM generation output to control motor speed");
	Shell_Add(h_shell, "PWM_Stop", PWM_Stop, "Stop PWM generation output to disable motor");
	Shell_Add(h_shell, "ADC_Read", ADC_Read, "Display ADC_values concerning motor currents.");



	HAL_UART_Receive_IT(&huart2, h_shell->uartRxBuffer, UART_RX_BUFFER_SIZE);
	HAL_UART_Transmit(&huart2, started, strlen((char *)started), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, prompt, strlen((char *)prompt), HAL_MAX_DELAY);
}

static int Shell_Exec(h_shell_t *h_shell,char **argv,int argc)
{
	// Lookup and execute the command
	char *user_func = argv[0]; // First token is the command
	for (int i = 0; i < h_shell->shell_func_list_size; i++)
	{
		if (strcmp(h_shell->shell_func_list[i].name, user_func) == 0)
		{
			// Execute the command
			int result = h_shell->shell_func_list[i].func(h_shell,argv,argc );

			// Clean up dynamically allocated memory
			for (int j = 0; j < argc; j++)
			{
				free(argv[j]);
			}
			return result;
		}
	}
	HAL_UART_Transmit(&huart2, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
	return 1;
}

void Shell_Loop(void){
	if(h_shell->uartRxReceived){
		switch(h_shell->uartRxBuffer[0]){
		case ASCII_CR: // Nouvelle ligne, instruction à traiter
			HAL_UART_Transmit(&huart2, newline, sizeof(newline), HAL_MAX_DELAY);
			h_shell->cmdBuffer[h_shell->idx_cmd] = '\0';
			argc = 0;
			token = strtok(h_shell->cmdBuffer, " ");
			while(token!=NULL){
				argv[argc++] = token;
				token = strtok(NULL, " ");
			}
			h_shell->idx_cmd = 0;
			newCmdReady = 1;
			break;
		case ASCII_BACK: // Suppression du dernier caractère
			h_shell->cmdBuffer[h_shell->idx_cmd--] = '\0';
			HAL_UART_Transmit(&huart2, backspace, sizeof(backspace), HAL_MAX_DELAY);
			break;

		default: // Nouveau caractère
			h_shell->cmdBuffer[h_shell->idx_cmd++] = h_shell->uartRxBuffer[0];
			HAL_UART_Transmit(&huart2, h_shell->uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
		}
		h_shell->uartRxReceived = 0;
	}

	if(newCmdReady){

		Shell_Exec(h_shell, argv,argc);
		newCmdReady = 0;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart){
	h_shell->uartRxReceived = 1;
	HAL_UART_Receive_IT(&huart2, h_shell->uartRxBuffer, UART_RX_BUFFER_SIZE);
}
