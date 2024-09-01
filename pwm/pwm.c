/*
 * pwm.c
 *
 *  Created on: 2024年8月30日
 *      Author: 12630
 */
/*
 *@Note
 PWM output routine:
 TIM1_CH3 （PC3）
    调背光
*/
#include "pwm.h"



//pc3
void TIM1_PWMOut_Init(u16 arr, u16 psc, u16 ccp)
{

    TIM_OCInitTypeDef TIM_OCInitStructure={0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_APB2PeriphClockCmd(   RCC_APB2Periph_TIM1, ENABLE );


    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);


    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
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


