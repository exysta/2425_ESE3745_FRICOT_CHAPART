/*
 * mesure_courant.c
 *
 *  Created on: Nov 15, 2024
 *      Author: exysta
 */

#include "mylibs/mesure_courant.h"
#include "adc.h"
#include "mylibs/shell.h"

#include "usart.h"
#include "tim.h"
#include <string.h> // Include the string.h header for memset
#include "stm32g4xx_hal.h"

uint8_t adc_value_available = 0;

uint16_t adc_values[ADC_BUFFER_SIZE];

// Initialize ADC and start conversion
int ADC_Start()
{
	// Clear ADC value arrays
	memset(adc_values, 0, ADC_BUFFER_SIZE);

	// Calibrate ADCs
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);

	// Set up DMA for ADCs
	HAL_ADC_Start_DMA(&hadc1, adc_values, ADC_BUFFER_SIZE);

	// Start the timer to trigger ADC conversions
	HAL_TIM_Base_Start(&htim1);

	return 0;
}

// Callback when ADC conversion is complete
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance == ADC1)
	{
		// ADC1 completed conversion
		adc_value_available = 1;
	}

}

// Example read function (extend as needed)
int ADC_Read(h_shell_t *h_shell, char **argv, int argcn)
{
	if (adc_value_available)
	{
		adc_value_available = 0;

		// Process ADC values (e.g., print or send to shell)

		int uartTxStringLength = snprintf((char*) h_shell->uartTxBuffer,
				UART_TX_BUFFER_SIZE, "U_imes = %d ,V_imes = %d ,Bus_imes = %d \r\n",
				(int) adc_values[0],(int) adc_values[1],(int) adc_values[2]);
		HAL_UART_Transmit(&huart2, h_shell->uartTxBuffer, uartTxStringLength,
				HAL_MAX_DELAY);


	}
	return 0;
}
