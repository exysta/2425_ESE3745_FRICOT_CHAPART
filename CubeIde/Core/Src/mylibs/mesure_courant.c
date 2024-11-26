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

uint8_t adc1_value_available = 0;
uint8_t adc2_value_available = 0;

uint16_t adc_phase_values[PHASE_QUANTITY];
uint16_t adc_bus_values[BUS_QUANTITY];

// Initialize ADC and start conversion
int ADC_Start()
{
    // Clear ADC value arrays
    memset(adc_phase_values, 0, sizeof(adc_phase_values));
    memset(adc_bus_values, 0, sizeof(adc_bus_values));

    // Calibrate ADCs
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);

    // Set up DMA for ADCs
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_phase_values, PHASE_QUANTITY);
    HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adc_bus_values, BUS_QUANTITY);

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
        adc1_value_available = 1;
    }
    else if (hadc->Instance == ADC2)
    {
        // ADC2 completed conversion
    	adc2_value_available  = 1;
    }
}

// Example read function (extend as needed)
int ADC_Read(h_shell_t *h_shell, char **argv, int argcn)
{
    if (adc1_value_available)
    {
    	adc1_value_available = 0;

        // Process ADC values (e.g., print or send to shell)
        for (int i = 0; i < PHASE_QUANTITY; i++)
        {
            printf("ADC1 Channel %d Value: %u\n", i, adc_phase_values[i]);
        }

        for (int i = 0; i < BUS_QUANTITY; i++)
        {
            printf("ADC2 Channel %d Value: %u\n", i, adc_bus_values[i]);
        }
    }
    return 0;
}

