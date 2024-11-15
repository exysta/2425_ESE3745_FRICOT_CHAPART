/*
 * mesure_courant.c
 *
 *  Created on: Nov 15, 2024
 *      Author: exysta
 */

#include "mylibs/mesure_courant.h"
#include "adc.h"
#include "usart.h"
#include "tim.h"
#include <string.h> // Include the string.h header for memset
#include "stm32g4xx_hal.h"

uint8_t adc_value_available = 0;
uint16_t adc_values[CHANNEL_QUANTITY];


void start_adc()
{
	memset(adc_values, 0, sizeof(adc_values)); // Set all values of adc_values to 0
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc1, adc_values, CHANNEL_QUANTITY);
	HAL_TIM_Base_Start(&htim1);
}
void mesure_courant()
{

    ADC_ChannelConfTypeDef sConfig = {0};
    // Example: Select ADC Channel 1 (assuming it's configured in CubeMX)
    if(adc_value_available)
    {

        adc_value_available = 0;
    }

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if(ADC1 == hadc->Instance)
	{
		adc_value_available = 1;
	}
}

