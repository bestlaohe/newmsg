/*
 * encode.c
 *
 *  Created on: 2024��9��1��
 *      Author: 12630
 */



/**
 * @note
 * Encoder rourine
 * The main function initializes and tests an encoder, and continuously prints the encoder position and
 * speed.
 */
#include "debug.h"
#include "encode.h"
/* Global define */

/* The `#define SpeedSampleTimeMs 20` line is defining a constant macro `SpeedSampleTimeMs` with a
value of 20. This constant is used in the code to specify the time interval (in milliseconds) at
which the speed of the encoder is sampled and calculated. */
/* Global Variable */




/*********************************************************************
 * @fn TIM1_Encoder_Init
 *
 * @brief function initializes the TIM1 timer as an encoder with specific settings.
 * TIM1_CH1 (PD2)
 * TIM1_CH3 (PC3)
 * @return none
 */
void TIM1_Encoder_Init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
    TIM_TimeBaseStructure.TIM_Period = 80;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    //d2a1
    // ���� TIM1 ���벶��
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1 | TIM_Channel_2; // �������벶��ͨ��Ϊͨ�� 1 ��ͨ�� 3N
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;      // �������벶��ļ���Ϊ�����ش���
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  // ѡ��ֱ�����루TI��
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;             // �������벶��Ԥ��Ƶ��Ϊ 1������Ƶ��
    TIM_ICInitStructure.TIM_ICFilter = 10;                            // �������벶���˲����Ĳ�������Ϊ 10�������˲�������
    TIM_ICInit(TIM1, &TIM_ICInitStructure);



    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    TIM_SetCounter(TIM1, 0);
    TIM_Cmd(TIM1, ENABLE);
}
