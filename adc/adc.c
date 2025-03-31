/*
 * adc.c
 *
 *  Created on: 2024年9月7日
 *      Author: 12630
 */

#include "adc.h"
#include "seting.h"


u16 BattaryBuf[ADC_CONUT];
// 关键点的 ADC 值和对应的电池百分比
#define NUM_POINTS 5
const uint16_t adc_points[NUM_POINTS] ={421, 477, 558, 609, 645}; // 示例关键点
const uint8_t percent_points[NUM_POINTS] = {0, 25, 50, 75, 100};   // 对应的百分比


//percentage=41 578
// percentage=32 562
// 4.2V ≈ 100%
// 3.6V ≈ 50%
// 3.0V ≈ 10%
// 2.7V ≈ 0%



// 查找表的大小
#define ADC_MAX 1023

// 函数：使用线性插值获取电池百分比
uint8_t get_battery_percentage(uint16_t adc_value)
{
    if (adc_value >= adc_points[NUM_POINTS - 1])
    {
        return percent_points[NUM_POINTS - 1];
    }

    // 查找点之间的插值
    for (u8 i = 0; i < NUM_POINTS - 1; i++)
    {
        if (adc_value >= adc_points[i] && adc_value < adc_points[i + 1])
        {
            // 线性插值公式
            uint16_t range = adc_points[i + 1] - adc_points[i];
            uint16_t delta = adc_value - adc_points[i];
            uint8_t percentage_range = percent_points[i + 1] - percent_points[i];
            uint8_t percentage = percent_points[i] + (delta * percentage_range) / range;
            if (percentage > 100)
                percentage = 100;
            return percentage;
        }
    }

    // 如果不在任何区间内，返回最小值
    return percent_points[0];
}

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

    DMA_Tx_Init(DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)BattaryBuf, ADC_CONUT); // 电量会一直存在这个buf里面
    DMA_Cmd(DMA1_Channel1, ENABLE);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_241Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  
}
void Battery_DeInit(void)
{
    // 停止ADC转换
    ADC_SoftwareStartConvCmd(ADC1, DISABLE); // 禁用软件启动转换

    // 禁用DMA
    DMA_Cmd(DMA1_Channel1, DISABLE); // 禁用 DMA 通道
    DMA_DeInit(DMA1_Channel1);       // 可选：如果希望重置DMA配置

    // 禁用ADC
    ADC_Cmd(ADC1, DISABLE); // 禁用 ADC1

    // 禁用ADC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE); // 禁用 ADC1 时钟
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
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             // DMA工作模式：循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     // 设置优先级：非常高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                // 禁用内存到内存模式
    DMA_Init(DMA_CHx, &DMA_InitStructure);                                      // 初始化指定的DMA通道
    DMA_ITConfig(DMA_CHx, DMA_IT_TC, ENABLE);                                   // 使能DMA传输完成中断
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); // 配置NVIC
}

void show_battery(UWORD Xpoint, UWORD Ypoint, UWORD Color_Background, UWORD Color_Foreground, u8 *needshow)
{

    static u8 percentage = 0;
    static u8 Prepercentage = 101;
    char strBuf[4]; // 要存储最多3位数字和一个终止符，所以数组大小为4

    percentage = get_battery_percentage(Battery_ADC_Average);
//   DEBUG_PRINT("percentage=%d %d\r\n",percentage,Battery_ADC_Average);
    // percentage = 100;

    // 限制百分比范围
    if (percentage > 100)
        percentage = 100;

    if(abs(Prepercentage - percentage) > 5 || *needshow)
    {
        *needshow = 0;
        Prepercentage = percentage;

        u8 cnt = percentage / 25;
        u16 strX = Xpoint - 2 * Font16_Num.Width; // 默认 2 位数的显示位置
        u16 color = BLUE;                         // 默认颜色

        if (percentage >= 100)
        {
            strX = Xpoint - 3 * Font16_Num.Width; // 3 位数的显示位置
            color = GREEN;                        // 颜色

            intToStr(percentage, strBuf, 3);
        }
        else if (percentage < 10)
        {

            Paint_DrawString(Xpoint - Font16_Num.Width * 3, Ypoint, "<<", &Font16_Num, Color_Background, Color_Background, '0',999);
            strX = Xpoint - Font16_Num.Width; // 1 位数的显示位置
            color = RED;                      // 颜色

            intToStr(percentage, strBuf, 1);
        }
        else
        {
            Paint_DrawString(Xpoint - Font16_Num.Width * 3, Ypoint, "<", &Font16_Num, Color_Background, Color_Background, '0',999);

            intToStr(percentage, strBuf, 2);
        }

        // 数字百分比
        Paint_DrawString(strX, Ypoint, strBuf, &Font16_Num, Color_Background, Color_Foreground, '0',999);
        Paint_DrawChar(Xpoint, Ypoint, 10, &Font16_Num, Color_Background, Color_Foreground, 0); //%

        if (charge.state == CHARGING)
        {
            Paint_DrawChar(Xpoint + Font16_Num.Width, Ypoint, 1, &Font16_Bat, Color_Background, color, 0); // 充电log
      
        }
        else if (charge.state == UNCHARGING)
        {
            Paint_DrawChar(Xpoint + Font16_Num.Width, Ypoint, 0, &Font16_Bat, Color_Background, color, 0); // 空电log
            for (u8 i = 0; i < cnt; i++)
            {
                Paint_DrawLine(108 + 4 + i * 3, 3 + Ypoint, 108 + 4 + i * 3, 7 + Ypoint, color, 1, LINE_STYLE_SOLID);
            }
   
        }
    }
}
