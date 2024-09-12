/*
 * adc.h
 *
 *  Created on: 2024Äê9ÔÂ7ÈÕ
 *      Author: 12630
 */

#ifndef ADC_ADC_H_
#define ADC_ADC_H_

#include "debug.h"
extern u16 BattaryBuf[10];

void Battery_Init(void);
u16 Get_ADC_Val(u8 ch);
void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize);

#endif /* ADC_ADC_H_ */
