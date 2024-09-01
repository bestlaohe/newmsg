/*
 * drive_screen.h
 *
 *  Created on: 2024Äê8ÔÂ30ÈÕ
 *      Author: 12630
 */

#ifndef SCREEN_DRIVE_SCREEN_H_
#define SCREEN_DRIVE_SCREEN_H_


#include "ch32v00x_gpio.h"
#include "pwm.h"


#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

#define DEV_Set_PWM(_Value)     TIM_SetCompare3(TIM1,_Value)

#define LCD_CS_0       GPIO_ResetBits(GPIOC, GPIO_Pin_4)
#define LCD_CS_1       GPIO_SetBits(GPIOC, GPIO_Pin_4)

#define LCD_RST_0    GPIO_ResetBits(GPIOD, GPIO_Pin_0)
#define LCD_RST_1     GPIO_SetBits(GPIOD, GPIO_Pin_0)

#define LCD_DC_0       GPIO_ResetBits(GPIOC, GPIO_Pin_1)
#define LCD_DC_1       GPIO_SetBits(GPIOC, GPIO_Pin_1)

#define LCD_SCK_0  GPIO_ResetBits(GPIOC, GPIO_Pin_5)
#define LCD_SCK_1   GPIO_SetBits(GPIOC, GPIO_Pin_5)

#define LCD_SDA_0  GPIO_ResetBits(GPIOC, GPIO_Pin_6)
#define LCD_SDA_1    GPIO_SetBits(GPIOC, GPIO_Pin_6)


#define LCD_BL_0   GPIO_ResetBits(GPIOC, GPIO_Pin_3)
#define LCD_BL_1   GPIO_SetBits(GPIOC, GPIO_Pin_3)


//*Slave:SPI1_NSS(PC1)¡¢SPI1_SCK(PC5)¡¢SPI1_MISO(PC7)¡¢SPI1_MOSI(PC6).
/* SPI Mode Definition */
#define HOST_MODE     0
#define SLAVE_MODE    1

/* SPI Communication Mode Selection */
#define SPI_MODE      HOST_MODE
//#define SPI_MODE   SLAVE_MODE

/* Global define */
#define Size          18

void SPI_FullDuplex_Init(void);
void DEV_SPI_WRite(uint8_t _dat);
int DEV_Module_Init(void);
void DEV_Module_Exit(void);
void LCD_0in85_test();


#endif /* SCREEN_DRIVE_SCREEN_H_ */
