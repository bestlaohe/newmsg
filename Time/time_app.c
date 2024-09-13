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

//100us触发一次中断
void TIM1_Init(u16 arr, u16 psc, u16 ccp){

        TIM_OCInitTypeDef TIM_OCInitStructure={0};
        TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};
        NVIC_InitTypeDef NVIC_InitStructure;
        RCC_APB2PeriphClockCmd(   RCC_APB2Periph_TIM1, ENABLE );

        TIM_TimeBaseInitStructure.TIM_Period = arr;
        TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
        TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);


        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
        TIM_OCInitStructure.TIM_Pulse = ccp;
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
        TIM_OC3Init( TIM1, &TIM_OCInitStructure );

        TIM_OC3PreloadConfig( TIM1, TIM_OCPreload_Disable );
        TIM_ARRPreloadConfig( TIM1, ENABLE );
        TIM_Cmd( TIM1, ENABLE );


        // 配置定时器中断
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);                 // 使能更新中断

    // 配置 NVIC 中断
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;          // 中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             // 使能中断

    NVIC_Init(&NVIC_InitStructure);
}

//作为倒计时10ms一次触发中断
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

int SleepCounter=0;
void refresh_SleepCounter(int newtime){

    SleepCounter=newtime;
}
void system_wokeup(){

  refresh_SleepCounter(0);//刷新休眠时间

}


// 定时器中断服务函数
void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {

        SleepCounter++;
       if (SleepCounter >= 150000) // 15s触发一次
       {
           printf("定时器15s了开始进入待机模式\r\n");
           SleepCounter = 0;
          // PWR_EnterSTANDBYMode(PWR_STANDBYEntry_WFI);
       }
       // 清除中断标志
         TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

//用于编码器
void TIM2_Init(u16 arr, u16 psc){

        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
        TIM_ICInitTypeDef TIM_ICInitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
        TIM_TimeBaseStructure.TIM_Prescaler = psc;//预分频器
        TIM_TimeBaseStructure.TIM_Period = arr;//自动重装载值
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;            //重复计数器值
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
        TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);


        // 配置 TIM1 输入捕获d3d4
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_1 | TIM_Channel_2; // 设置输入捕获通道为通道 1 和通道 3N
        TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;      // 设置输入捕获的极性为上升沿触发
        TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  // 选择直接输入（TI）
        TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;             // 设置输入捕获预分频器为 1（不分频）
        TIM_ICInitStructure.TIM_ICFilter = 10;                            // 设置输入捕获滤波器的采样周期为 10（用于滤波抖动）
        TIM_ICInit(TIM2, &TIM_ICInitStructure);


        NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

        NVIC_Init(&NVIC_InitStructure);

        TIM_ClearFlag(TIM2, TIM_FLAG_Update);
        TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
        TIM_SetCounter(TIM2, 0);
        TIM_Cmd(TIM2, ENABLE);

}
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler()
{

  volatile uint16_t tempcnt = TIM2->CNT, temparr = TIM2->ATRLR;

  if (TIM_GetITStatus(TIM2, TIM_IT_Update))
  {
      printf("当前计数=%d\r\n",tempcnt);
      printf("重装载值=%d\r\n",temparr);

      if (tempcnt < temparr / 2)
      {
          circle += 1;
      }
      else
      {
          circle -= 1;
      }
  }
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}
