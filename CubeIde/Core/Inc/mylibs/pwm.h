/*
 * pwm.h
 *
 *  Created on: Nov 8, 2024
 *      Author: exysta
 */

#ifndef INC_MYLIBS_PWM_H_
#define INC_MYLIBS_PWM_H_


#define MAX_PULSE 8499
#define NEUTRAL_PULSE 4249 // pulse auquel le moteur ne tourne pas
#define RAMP_TIME 6000 //temps en milisecondes pour changer de pulse
void PWM_set_pulse(uint32_t speed);
void PWM_Start();
void PWM_Stop();

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
