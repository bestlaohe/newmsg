/*
 * pwm.h
 *
 *  Created on: 2024��8��30��
 *      Author: 12630
 */

#ifndef PWM_PWM_H_
#define PWM_PWM_H_
#include "ch32v00x_rcc.h"

void TIM1_PWMOut_Init(u16 arr, u16 psc, u16 ccp);
void PWM_Config(u16 Freq, u16 Duty);


#endif /* PWM_PWM_H_ */
