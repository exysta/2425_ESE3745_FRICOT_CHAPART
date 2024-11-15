/*
 * pwm.h
 *
 *  Created on: Nov 8, 2024
 *      Author: exysta
 */

#ifndef INC_MYLIBS_PWM_H_
#define INC_MYLIBS_PWM_H_

#include "shell.h"

#define MAX_PULSE 8499
#define NEUTRAL_PULSE 4249 // pulse auquel le moteur ne tourne pas
#define RAMP_TIME 6000 //temps en milisecondes pour changer de pulse
int PWM_Set_Pulse(uint32_t speed);
int PWM_Speed_Control(h_shell_t *h_shell,char **argv,int argc);
int PWM_Start(h_shell_t *h_shell,char **argv,int argc);
int PWM_Stop(h_shell_t *h_shell,char **argv,int argc);

// pwm.h
// Define a struct to hold PWM-related variables
typedef struct {
    uint16_t pulse1;
    uint16_t pulse2;
    uint16_t previous_pulse1;
    uint16_t previous_pulse2;
    uint16_t intermediate_pulse1;
    uint16_t intermediate_pulse2;
    uint16_t interrupt_counter;
} PWM_HandleTypeDef;



#endif /* INC_MYLIBS_PWM_H_ */
