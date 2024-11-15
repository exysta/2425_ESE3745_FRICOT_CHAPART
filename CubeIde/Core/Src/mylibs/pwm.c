/*
 * pwm.c
 *
 *  Created on: Nov 8, 2024
 *      Author: exysta
 */

#include <stdio.h>
#include "usart.h"
#include "mylibs/pwm.h"
#include "mylibs/shell.h"
#include "tim.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

PWM_HandleTypeDef pwm_handle;
uint32_t speed; // between 0-100 for motor speed. 0 is max speed in reverse, 50 is neutral and 100 is max speed in the other direction.
int PWM_Set_Pulse(uint32_t speed)
{
	//on disable l'intteruption pour éviter la modification des variables global dans l'interrupt du timer 7
    __disable_irq();  // Disable interrupts
    pwm_handle.pulse1 = MAX_PULSE* speed/100;
    pwm_handle.pulse2 = MAX_PULSE - pwm_handle.pulse1;
    pwm_handle.previous_pulse1 = htim1.Instance->CCR1;
    pwm_handle.previous_pulse2 = htim1.Instance->CCR2;
	 __enable_irq();   // Enable interrupts

	 pwm_handle.interrupt_counter = 0;
	 return 0;
}

int PWM_Speed_Control(h_shell_t *h_shell,char **argv,int argc)
{
	uint32_t new_speed = atoi(argv[1]);//speed in expected in % of max speed

	if(argc != 2)
	{
		uint8_t error_message[] = "Error : speed function expect exactly 1 parameter \r\n";
		HAL_UART_Transmit(&huart2, error_message, sizeof(error_message), HAL_MAX_DELAY);

		return 1;
	}

	else if(speed > 90 || speed < 10)//on vérifie qu'on met pas la vitesse ne soit pas au dessus de 95% de la max par sécurité
	{
		uint8_t error_message[] = "speed function must not exceed 90% of max value  \r\n";
		HAL_UART_Transmit(&huart2, error_message, sizeof(error_message), HAL_MAX_DELAY);
		return 1;

	}
	else if((speed < 50 && new_speed > 50 )|| (speed > 50 && new_speed < 50 ))
	{
		uint8_t error_message[] = "speed function must not change the direction of rotation, please go to neutral by entering 50 before\r\n";
		HAL_UART_Transmit(&huart2, error_message, sizeof(error_message), HAL_MAX_DELAY);
		return 1;
	}
	speed = new_speed;
	PWM_Set_Pulse(speed);
	int uartTxStringLength = snprintf((char *)h_shell->uartTxBuffer, UART_TX_BUFFER_SIZE, "speed set to %lu of max value \r\n",(unsigned long)speed);
	HAL_UART_Transmit(&huart2, h_shell->uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
	return 0;


}

int PWM_Start(h_shell_t *h_shell,char **argv,int argc)
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

	pwm_handle.interrupt_counter = 0;

	pwm_handle.previous_pulse1 = htim1.Instance->CCR1;
	pwm_handle.previous_pulse2 = htim1.Instance->CCR2;
	pwm_handle.pulse1 = htim1.Instance->CCR1;
	pwm_handle.pulse2 = htim1.Instance->CCR2;

	HAL_TIM_Base_Start_IT(&htim7);
	return 0;

}

int PWM_Stop(h_shell_t *h_shell,char **argv,int argc)
{
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIM_Base_Stop(&htim7);
	return 0;

}

