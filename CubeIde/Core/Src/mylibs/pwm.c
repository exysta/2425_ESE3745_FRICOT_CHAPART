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

void set_pulse(uint32_t speed)
{
	uint16_t pulse1 = MAX_PULSE* speed/100;
	uint16_t pulse2 = MAX_PULSE - pulse1;
	for(int i = 0;i>20;i++)
	{
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (i+1/20)*pulse1);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (i+1/20)*pulse2);
		//on doit genéréer timer pour faire varier le temps.
		//0-100% en 5 sec pour moteur
	}

}
