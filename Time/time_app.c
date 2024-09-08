/*
 * time.c
 *
 *  Created on: 2024��9��1��
 *      Author: 12630
 */

#include "time_app.h"
void TIM1_Init(u16 arr, u16 psc, u16 ccp){

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

void TIM2_Init(u16 arr, u16 psc){

        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
        TIM_ICInitTypeDef TIM_ICInitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
        TIM_TimeBaseStructure.TIM_Prescaler = psc;
        TIM_TimeBaseStructure.TIM_Period = arr;
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;            //�ظ�������ֵ
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
        TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);


        // ���� TIM1 ���벶��d3d4
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_1 | TIM_Channel_2; // �������벶��ͨ��Ϊͨ�� 1 ��ͨ�� 3N
        TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;      // �������벶��ļ���Ϊ�����ش���
        TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  // ѡ��ֱ�����루TI��
        TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;             // �������벶��Ԥ��Ƶ��Ϊ 1������Ƶ��
        TIM_ICInitStructure.TIM_ICFilter = 10;                            // �������벶���˲����Ĳ�������Ϊ 10�������˲�������
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
