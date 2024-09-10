/*
 * adc.c
 *
 *  Created on: 2024年9月7日
 *      Author: 12630
 */

#include "adc.h"

/*********************************************************************
 * @fn      Battery_Init
 *
 * @brief   Initializes ADC collection.
 *
 * @return  none
 */
void Battery_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // 开启adc1

    RCC_ADCCLKConfig(RCC_PCLK2_Div8); // 配置ADC时钟为PCLK2的1/8

    ADC_DeInit(ADC1); // 复位ADC1寄存器到默认状态

    // 初始化ADC配置结构体
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  // 设置ADC为独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                       // 禁用扫描转换模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                  // 启用连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 无外部触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              // 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;                             // 设置转换通道数量为1
    ADC_Init(ADC1, &ADC_InitStructure);                                 // 初始化ADC1，应用配置结构体

    ADC_Calibration_Vol(ADC1, ADC_CALVOL_50PERCENT); // 校准ADC，使用50%参考电压
    ADC_DMACmd(ADC1, ENABLE);                        // 启用DMA以支持ADC数据传输
    ADC_Cmd(ADC1, ENABLE);                           // 启动ADC
    ADC_ResetCalibration(ADC1);                      // 重置ADC校准
    while (ADC_GetResetCalibrationStatus(ADC1))
        ;                       // 等待校准重置完
    ADC_StartCalibration(ADC1); // 启动ADC校准
    while (ADC_GetCalibrationStatus(ADC1))
        ; // 等待校准完成

    DMA_Tx_Init(DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)BattaryBuf, 10); // 电量会一直存在这个buf里面
    DMA_Cmd(DMA1_Channel1, ENABLE);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_241Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    // Delay_Ms(50);
    // ADC_SoftwareStartConvCmd(ADC1, DISABLE);//休眠模式的时候降低功耗用
}

/*********************************************************************
 * @fn      Get_ADC_Val
 *
 * @brief   Returns ADCx conversion result data.
 *
 * @param   ch - ADC channel.
 *            ADC_Channel_0 - ADC Channel0 selected.
 *            ADC_Channel_1 - ADC Channel1 selected.
 *            ADC_Channel_2 - ADC Channel2 selected.
 *            ADC_Channel_3 - ADC Channel3 selected.
 *            ADC_Channel_4 - ADC Channel4 selected.
 *            ADC_Channel_5 - ADC Channel5 selected.
 *            ADC_Channel_6 - ADC Channel6 selected.
 *            ADC_Channel_7 - ADC Channel7 selected.
 *            ADC_Channel_8 - ADC Channel8 selected.
 *            ADC_Channel_9 - ADC Channel9 selected.
 *
 * @return  none
 */
u16 Get_ADC_Val(u8 ch)
{
    u16 val;

    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_241Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;
    val = ADC_GetConversionValue(ADC1);

    return val;
}

/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);                                                        // 取消初始化指定的DMA通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;                           // 设置外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;                              // 设置内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                          // 数据传输方向：从外设到内存
    DMA_InitStructure.DMA_BufferSize = bufsize;                                 // 设置传输数据的缓冲区大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 外设数据大小：半字
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         // 内存数据大小：半字
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                               // DMA工作模式：循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     // 设置优先级：非常高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                // 禁用内存到内存模式
    DMA_Init(DMA_CHx, &DMA_InitStructure);                                      // 初始化指定的DMA通道
}
