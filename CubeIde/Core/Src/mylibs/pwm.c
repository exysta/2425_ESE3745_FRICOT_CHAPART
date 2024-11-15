/*
 * pwm.c
 *
 *  Created on: Nov 8, 2024
 *      Author: exysta
 */

#include <stdio.h>
#include "usart.h"
#include "mylibs/pwm.h"
#include "tim.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

PWM_HandleTypeDef pwm_handle;

void PWM_set_pulse(uint32_t speed)
{
	//on disable l'intteruption pour éviter la modification des variables global dans l'interrupt du timer 7
    __disable_irq();  // Disable interrupts
    pwm_handle.pulse1 = MAX_PULSE* speed/100;
    pwm_handle.pulse2 = MAX_PULSE - pwm_handle.pulse1;
    pwm_handle.previous_pulse1 = htim1.Instance->CCR1;
    pwm_handle.previous_pulse2 = htim1.Instance->CCR2;
	 __enable_irq();   // Enable interrupts

	 pwm_handle.interrupt_counter = 0;
}

void PWM_Start()
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

}

void PWM_Stop()
{
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIM_Base_Stop(&htim7);

}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM6) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */
	if (htim->Instance == TIM7)
	{
		if(pwm_handle.interrupt_counter < RAMP_TIME - 1) //on update la valeur de pulse chaque miliseconde
		{
			pwm_handle.intermediate_pulse1 = pwm_handle.previous_pulse1 + (pwm_handle.pulse1 - pwm_handle.previous_pulse1)  * (pwm_handle.interrupt_counter+1)/RAMP_TIME ;
			pwm_handle.intermediate_pulse2 = pwm_handle.previous_pulse2 + (pwm_handle.pulse2 - pwm_handle.previous_pulse2)  * (pwm_handle.interrupt_counter+1)/RAMP_TIME ;

			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,pwm_handle.intermediate_pulse1);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,pwm_handle.intermediate_pulse2);

			pwm_handle.interrupt_counter++;
		}

	}
	/* USER CODE END Callback 1 */
}
