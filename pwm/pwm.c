/*
 * pwm.c
 *
 *  Created on: 2024��8��30��
 *      Author: 12630
 */
/*
 *@Note
 PWM output routine:
 TIM1_CH3 ��PC3��
    ������
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
    // ȷ��Ƶ���ں���Χ��
    if (Freq == 0) return; // ���������
    if (Duty > 100) Duty = 100; // ȷ��ռ�ձȲ�����100%

    // ����Ԥ��Ƶ�����Զ����ؼĴ���ֵ
    u16 psc = (SystemCoreClock / (100*Freq)) - 1; // ����Ԥ��Ƶ��48

    // ���ó�ʼ������
    TIM1_PWMOut_Init(100, psc, Duty);
}


