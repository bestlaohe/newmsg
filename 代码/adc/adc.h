/*
 * adc.h
 *
 *  Created on: 2024年9月7日
 *      Author: 12630
 */

#ifndef ADC_ADC_H_
#define ADC_ADC_H_

#include "debug.h"
#include "Screen_api.h"
#include <ch32v00x_it.h>

extern u16 Battery_ADC_Average;
extern Charge charge ;
extern u8 isFirstBattaryShow; // 电池刷新标志
void Battery_Init(void);
void Battery_DeInit(void);
u16 Get_ADC_Val(u8 ch);
void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize);
uint8_t get_battery_percentage(uint16_t adc_value);
void show_battery(UWORD Xpoint, UWORD Ypoint,UWORD Color_Background, UWORD Color_Foreground);
uint16_t Get_Calibrated_ADC(uint16_t adc_value);
#endif /* ADC_ADC_H_ */
