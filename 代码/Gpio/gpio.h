/*
 * gpio.h
 *
 *  Created on: 2024锟斤拷9锟斤拷1锟斤拷
 *      Author: 12630
 */

#ifndef GPIO_GPIO_H_
#define GPIO_GPIO_H_
#include "ch32v00x_gpio.h"

void My_GPIO_Init();
void My_GPIO_DeInit();
void MOTOR_SET(int state);
#define MOTOR_ON GPIO_ResetBits(GPIOA, GPIO_Pin_2)
#define MOTOR_OFF GPIO_SetBits(GPIOA, GPIO_Pin_2)

#define LCD_CS_ENABLE GPIO_ResetBits(GPIOC, GPIO_Pin_4)
#define LCD_CS_DISABLE GPIO_SetBits(GPIOC, GPIO_Pin_4)

#define LCD_RST_0 GPIO_ResetBits(GPIOD, GPIO_Pin_7)
#define LCD_RST_1 GPIO_SetBits(GPIOD, GPIO_Pin_7)

#define LCD_DC_0 GPIO_ResetBits(GPIOC, GPIO_Pin_2)
#define LCD_DC_1 GPIO_SetBits(GPIOC, GPIO_Pin_2)

#define LCD_SCK_0 GPIO_ResetBits(GPIOC, GPIO_Pin_5)
#define LCD_SCK_1 GPIO_SetBits(GPIOC, GPIO_Pin_5)

#define LCD_SDA_0 GPIO_ResetBits(GPIOC, GPIO_Pin_6)
#define LCD_SDA_1 GPIO_SetBits(GPIOC, GPIO_Pin_6)

#define LCD_BL_0 GPIO_ResetBits(GPIOC, GPIO_Pin_3)
#define LCD_BL_1 GPIO_SetBits(GPIOC, GPIO_Pin_3)

#define SX1278_GPIO GPIOC      // GPIO选择
#define SX1278_NSS GPIO_Pin_0  // 引脚定义
#define SX1278_MOSI GPIO_Pin_6 // 引脚定义
#define SX1278_MISO GPIO_Pin_7 // 引脚定义
#define SX1278_SCK GPIO_Pin_5  // 引脚定义
#define SX1278_RST GPIO_Pin_0  // 复位引脚定义
#define SX1278_NIRQ GPIO_Pin_6 // 引脚定义

#define SX1278_NSS_DISABLE GPIO_SetBits(GPIOD, SX1278_NSS)
#define SX1278_NSS_ENABLE GPIO_ResetBits(GPIOD, SX1278_NSS) // 使能
#define READ_SX1278_MISO() GPIO_ReadInputDataBit(SX1278_GPIO, SX1278_MISO)
#define SET_SX1278_SCK() GPIO_SetBits(SX1278_GPIO, SX1278_SCK)
#define CLR_SX1278_SCK() GPIO_ResetBits(SX1278_GPIO, SX1278_SCK)
#define SET_SX1278_MOSI() GPIO_SetBits(SX1278_GPIO, SX1278_MOSI)
#define CLR_SX1278_MOSI() GPIO_ResetBits(SX1278_GPIO, SX1278_MOSI)
#define SET_SX1278_RST() GPIO_SetBits(SX1278_GPIO, SX1278_RST)       // 复位引脚定义
#define CLR_SX1278_RST() GPIO_ResetBits(SX1278_GPIO, SX1278_RST)     // 用于将 SX1278 模块置于关机模式
#define READ_SX1278_NIRQ() GPIO_ReadInputDataBit(GPIOD, SX1278_NIRQ) // 引脚是 SX1278 模块的中断请求引脚

#define READ_LCD_CS GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_4)
#define READ_LORA_CS GPIO_ReadOutputDataBit(GPIOD, SX1278_NSS)
#define KEY0 GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)
#define CHARGE GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)

#endif /* GPIO_GPIO_H_ */
