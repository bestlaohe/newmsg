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
#include <ch32v00x_dma.h>


#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

#define DEV_Set_PWM(_Value)     TIM_SetCompare3(TIM1,_Value)




//*Slave:SPI1_NSS(PC1)¡¢SPI1_SCK(PC5)¡¢SPI1_MISO(PC7)¡¢SPI1_MOSI(PC6).
/* SPI Mode Definition */
#define HOST_MODE     0
#define SLAVE_MODE    1

/* SPI Communication Mode Selection */
#define SPI_MODE      HOST_MODE

/* Global define */



void SPI_FullDuplex_Init(void);
u8 Screen_spi_write(uint8_t _dat);

void SPI_DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize,uint32_t mode,uint32_t MemoryInc);

#endif /* SCREEN_DRIVE_SCREEN_H_ */
