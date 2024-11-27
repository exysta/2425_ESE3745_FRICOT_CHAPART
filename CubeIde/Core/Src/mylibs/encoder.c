/*
 * encoder.c
 *
 *  Created on: Nov 27, 2024
 *      Author: exysta
 */

#include "tim.h"
#include "usart.h"

#include "math.h"
#include "mylibs/shell.h"

uint32_t rotation_sign;
float pulses_per_second;
#define TIMER_MAX_COUNT    65535 // Timer max count (16-bit)
#define DELTA_T 1000
int32_t last_count;
int32_t current_count;
//this function is called every second/10
void calculate_motor_speed()
{
	current_count = __HAL_TIM_GET_COUNTER(&htim3);
	int32_t delta_count;

	// Handle counter overflow/underflow
	if (current_count < last_count) {
		delta_count = (TIMER_MAX_COUNT + current_count) - last_count;
	} else {
		delta_count = current_count - last_count;
	}

	// Update last count
	last_count = current_count;
	pulses_per_second = delta_count * 10;
	rotation_sign = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3);
	// Calculate speed (RPM)
}

void ENCODER_print_speed(h_shell_t *h_shell, char **argv, int argc)
{
	if(rotation_sign == 1)
	{
		uint8_t update_message[] =
				"Counting Up!  \r\n";
		HAL_UART_Transmit(&huart2, update_message, sizeof(update_message),
				HAL_MAX_DELAY);
	}
	else{


		uint8_t update_message[] =
				"Counting Down!  \r\n";
		HAL_UART_Transmit(&huart2, update_message, sizeof(update_message),
				HAL_MAX_DELAY);

	}
	int uartTxStringLength = snprintf((char*) h_shell->uartTxBuffer,
			UART_TX_BUFFER_SIZE, "pulse per seconds %d \r\n",
			(int) pulses_per_second);
	HAL_UART_Transmit(&huart2, h_shell->uartTxBuffer, uartTxStringLength,
			HAL_MAX_DELAY);

}
void ENCODER_start(h_shell_t *h_shell, char **argv, int argc)
{
	rotation_sign = 0;
	pulses_per_second = 0;
	last_count=0;
	current_count=0;
	__HAL_TIM_SET_COUNTER(&htim3,0);
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	HAL_TIM_Base_Start_IT(&htim16);

	uint8_t update_message[] =
			"Encoder started !  \r\n";
	HAL_UART_Transmit(&huart2, update_message, sizeof(update_message),
			HAL_MAX_DELAY);
}

