/*
 * time.h
 *
 *  Created on: 2024Äê9ÔÂ1ÈÕ
 *      Author: 12630
 */

#ifndef TIME_TIME_APP_H_
#define TIME_TIME_APP_H_
#include "ch32v00x_rcc.h"

void TIM1_Init(u16 arr, u16 psc, u16 ccp);
void TIM2_Init(u16 arr, u16 psc);
void refresh_SleepCounter(int newtime);

void system_wokeup();



extern volatile int circle;
#endif /* TIME_TIME_APP_H_ */
