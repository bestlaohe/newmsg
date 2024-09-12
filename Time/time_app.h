/*
 * time.h
 *
 *  Created on: 2024��9��1��
 *      Author: 12630
 */

#ifndef TIME_TIME_APP_H_
#define TIME_TIME_APP_H_
#include "ch32v00x_rcc.h"

void TIM1_Init(u16 arr, u16 psc, u16 ccp);
void TIM2_Init(u16 arr, u16 psc);
extern volatile int circle;
extern int SleepCounter;
#endif /* TIME_TIME_APP_H_ */
