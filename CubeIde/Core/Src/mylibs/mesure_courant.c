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
uint16_t adc_phase_values[PHASE_QUANTITY];
uint16_t adc_bus_values[BUS_QUANTITY];

int ADC_Start()
{
	memset(adc_phase_values, 0, sizeof(adc_phase_values)); // Set all values of adc_values to 0
	memset(adc_bus_values, 0, sizeof(adc_bus_values)); // Set all values of adc_values to 0

	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc1, ( uint32_t *)adc_phase_values, PHASE_QUANTITY);
	HAL_ADC_Start_DMA(&hadc2, ( uint32_t *)adc_bus_values, BUS_QUANTITY);

	HAL_TIM_Base_Start(&htim1);
	return 0;
}

int ADC_Read(h_shell_t *h_shell,char **argv,int argcn)
{

	return 0;
}

//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
//{
//	if(ADC1 == hadc->Instance)
//	{
//		adc_value_available = 1;
//	}
//}

