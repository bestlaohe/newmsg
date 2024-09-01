/*
 * pwm.c
 *
 *  Created on: 2024年8月30日
 *      Author: 12630
 */
/*
 *@Note
 PWM output routine:
 TIM1_CH1(PD2)      我需要pc3
 This example demonstrates that the TIM_CH1(PD2) pin outputs PWM in PWM mode 1
 and PWM mode 2.

*/
#include "pwm.h"
/* PWM Output Mode Definition */
#define PWM_MODE1   0
#define PWM_MODE2   1

/* PWM Output Mode Selection */
//#define PWM_MODE PWM_MODE1
#define PWM_MODE PWM_MODE1

/*********************************************************************
 * @fn      TIM1_OutCompare_Init
 *
 * @brief   Initializes TIM1 output compare.
 *
 * @param   arr - the period value.100
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *  TIM_Period 表示定时器的周期值，这个值决定了PWM信号的周期。TIM_PrescalerTIM\_PrescalerTIM_Prescaler 表示定时器的预分频值，用于调整定时器的时钟频率
 * @return  none
 */


//pc3
void TIM1_PWMOut_Init(u16 arr, u16 psc, u16 ccp)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    TIM_OCInitTypeDef TIM_OCInitStructure={0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM1, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

#if (PWM_MODE == PWM_MODE1)
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

#elif (PWM_MODE == PWM_MODE2)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;

#endif

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC3Init( TIM1, &TIM_OCInitStructure );

    TIM_OC3PreloadConfig( TIM1, TIM_OCPreload_Disable );
    TIM_ARRPreloadConfig( TIM1, ENABLE );
    TIM_Cmd( TIM1, ENABLE );

}

void PWM_Config(u16 Freq, u16 Duty)//5000   50
{
    // 确保频率在合理范围内
    if (Freq == 0) return; // 避免除以零
    if (Duty > 100) Duty = 100; // 确保占空比不超过100%

    // 计算预分频器和自动重载寄存器值
    u16 psc = (SystemCoreClock / (100*Freq)) - 1; // 计算预分频器48

    // 调用初始化函数
    TIM1_PWMOut_Init(100, psc, Duty);
}


