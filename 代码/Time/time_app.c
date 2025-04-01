/*
 * time.c
 *
 *  Created on: 2024年9月1日
 *      Author: 12630
 */

#include "time_app.h"
/**
 * @brief   初始化 TIM1 定时器，用于生成 PWM 输出。
 *
 * @param   arr   定时器自动重装载寄存器 (ARR) 的值，决定 PWM 信号的周期。
 * @param   psc   定时器预分频器 (PSC) 的值，决定定时器计数频率。
 * @param   ccp   比较寄存器 (CCR) 的值，决定 PWM 信号的占空比。
 *
 * @note    配置 TIM1 为 PWM 输出模式，PWM 通道使用 TIM1_CH3 (pc3) 引脚。
 *          TIM1 被设置为向上计数模式，PWM 模式 2，输出高电平时有效。
 *          TIM1 的自动重装载寄存器和预分频器被初始化为用户传入的参数值。
 *          定时器计数到 ARR 值时会自动重载并产生更新事件。
 *
 * @retval  None
 */

// 100us触发一次中断
void TIM1_Init(u16 arr, u16 psc, u16 ccp)
{

    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);

    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);

    // 配置定时器中断
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE); // 使能更新中断

    // 配置 NVIC 中断
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;        // 中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能中断

    NVIC_Init(&NVIC_InitStructure);
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
}
void TIM1_DeInit(void)
{
    // 禁用定时器
    TIM_Cmd(TIM1, DISABLE);

    // 禁用定时器中断
    TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);

    // 复位定时器
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, DISABLE);

    // 清除定时器更新标志
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
}

// 用于编码器
void TIM2_Init(u16 arr, u16 psc)
{

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = psc; // 预分频器
    TIM_TimeBaseStructure.TIM_Period = arr;    // 自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; // 重复计数器值
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    // 配置 TIM1 输入捕获d3d4
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1 | TIM_Channel_2; // 设置输入捕获通道为通道 1 和通道 3N
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;      // 设置输入捕获的极性为上升沿触发
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  // 选择直接输入（TI）
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;            // 设置输入捕获预分频器为 1（不分频）
    TIM_ICInitStructure.TIM_ICFilter = 10;                           // 设置输入捕获滤波器的采样周期为 10（用于滤波抖动）
    TIM_ICInit(TIM2, &TIM_ICInitStructure);                          // 3，4捕获被我禁用了

    // 清除更新中断标志
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    TIM_SetCounter(TIM2, 0);

    TIM_Cmd(TIM2, ENABLE);
}
void TIM2_DeInit(void)
{
    // 禁用定时器
    TIM_Cmd(TIM2, DISABLE);

    // 禁用中断
    TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);

    // 复位定时器
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, DISABLE);

    // 清除更新标志
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
}
/*********************************************************************
 * @fn      IWDG_Feed_Init
 *
 * @brief   初始化 IWDG（独立看门狗）。
 *
 * @param   prer: 指定 IWDG 的预分频器值。
 *            IWDG_Prescaler_4: IWDG 预分频器设置为 4。
 *            IWDG_Prescaler_8: IWDG 预分频器设置为 8。
 *            IWDG_Prescaler_16: IWDG 预分频器设置为 16。
 *            IWDG_Prescaler_32: IWDG 预分频器设置为 32。
 *            IWDG_Prescaler_64: IWDG 预分频器设置为 64。
 *            IWDG_Prescaler_128: IWDG 预分频器设置为 128。
 *            IWDG_Prescaler_256: IWDG 预分频器设置为 256。
 *          rlr: 指定 IWDG 的重装载值。
 *            该参数必须是介于 0 和 0x0FFF 之间的一个数值。
 *
 * @return  none
 */
void IWDG_Feed_Init(u16 prer, u16 rlr)
{

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); // 使能 IWDG 写访问，以便可以设置 IWDG 的配置寄存器
    IWDG_SetPrescaler(prer);                      // 设置 IWDG 的预分频器值
    IWDG_SetReload(rlr);                          // 设置 IWDG 的重装载值，这个值决定了 IWDG 的超时时间
    IWDG_ReloadCounter();                         // 重新加载 IWDG 计数器，以使计数器从新的重装载值开始
    IWDG_Enable();                                // 启用 IWDG，使其开始工作
}

//128khz
// 以EVT例程10240分频为例，10240分频之后大约是12.5Hz。
// 此外，AWU有一个唤醒窗口寄存器，可配置窗口值，该值是6位的，最大可配置是0x3F，即是63，例程配置的是25。
// 10240分频之后是12.5Hz，计数一次的时间为1/12.5Hz，窗口值设置是25，则唤醒时间为25/12.5大约是2s左右。
// 若要加大唤醒时间间隔，可可知最大61440分频，然后窗口值设置为最大63.
void AWU_Init()
{

    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
        ;
    PWR_AWU_SetPrescaler(PWR_AWU_Prescaler_10240);
    PWR_AWU_SetWindowValue(25);
    PWR_AutoWakeUpCmd(ENABLE);
}