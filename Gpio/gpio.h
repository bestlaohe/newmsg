/*
 * gpio.h
 *
 *  Created on: 2024��9��1��
 *      Author: 12630
 */

#ifndef GPIO_GPIO_H_
#define GPIO_GPIO_H_
#include "ch32v00x_gpio.h"

void My_GPIO_Init();
#define MOTOR_ON   GPIO_ResetBits(GPIOA, GPIO_Pin_2)
#define MOTOR_OFF   GPIO_SetBits(GPIOA, GPIO_Pin_2)


#endif /* GPIO_GPIO_H_ */
