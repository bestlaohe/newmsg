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

#define KEY0 GPIO_ReadInputDataBit( GPIOD, GPIO_Pin_2)

#define CHARGING GPIO_ReadInputDataBit( GPIOD, GPIO_Pin_7)


#define LCD_CS_0       GPIO_ResetBits(GPIOC, GPIO_Pin_4)
#define LCD_CS_1       GPIO_SetBits(GPIOC, GPIO_Pin_4)

#define LCD_RST_0    GPIO_ResetBits(GPIOD, GPIO_Pin_0)
#define LCD_RST_1     GPIO_SetBits(GPIOD, GPIO_Pin_0)

#define LCD_DC_0       GPIO_ResetBits(GPIOC, GPIO_Pin_2)
#define LCD_DC_1       GPIO_SetBits(GPIOC, GPIO_Pin_2)

#define LCD_SCK_0  GPIO_ResetBits(GPIOC, GPIO_Pin_5)
#define LCD_SCK_1   GPIO_SetBits(GPIOC, GPIO_Pin_5)

#define LCD_SDA_0  GPIO_ResetBits(GPIOC, GPIO_Pin_6)
#define LCD_SDA_1    GPIO_SetBits(GPIOC, GPIO_Pin_6)


#define LCD_BL_0   GPIO_ResetBits(GPIOC, GPIO_Pin_3)
#define LCD_BL_1   GPIO_SetBits(GPIOC, GPIO_Pin_3)



#endif /* GPIO_GPIO_H_ */
