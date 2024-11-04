/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v00x_it.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/25
 * Description        : Main Interrupt Service Routines.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include <ch32v00x_it.h>
#include "adc.h"
void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast"))); // ��Ϊ����ʱ10msһ�δ����ж�
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

volatile int dmaTransferComplete = 0;
volatile int loraComplete = 0;

volatile int circle = 0;
int SleepCounter = 0;
int needSleep = 0;
int needDeinit = 0;
Encode encode = {ENCODE_EVENT_NONE};
Key key = {KEY_STATE_IDLE, KEY_EVENT_NONE, 0, 0, 1};
Charge charge = {UNCHARGING};

void TIM2_IRQHandler()
{

  volatile uint16_t tempcnt = TIM2->CNT, temparr = TIM2->ATRLR;

  if (TIM_GetITStatus(TIM2, TIM_IT_Update))
  {

    //     DEBUG_PRINT("��ǰ����=%d\r\n", tempcnt);
    //     DEBUG_PRINT("��װ��ֵ=%d\r\n", temparr);

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
void refresh_SleepCounter(int newtime)
{
  SleepCounter = newtime;
}

void DMA1_Channel3_IRQHandler(void)
{
  if (DMA_GetITStatus(DMA1_IT_TC3))
  {
    //   DEBUG_PRINT("dmaTransferComplete\r\n");
    dmaTransferComplete = 1;
    DMA_ClearITPendingBit(DMA1_IT_TC3); // ����жϱ�־
  }
}
u16 Battery_ADC_Average = 0;
void DMA1_Channel1_IRQHandler(void)
{

  if (DMA_GetITStatus(DMA1_IT_TC1))
  {
    // ������ƽ��ֵ
    for (u8 i = 0; i < 10; i++)
    {
      Battery_ADC_Average += BattaryBuf[i];
    }
    Battery_ADC_Average /= 10;          // ��ƽ��ֵ
    DMA_ClearITPendingBit(DMA1_IT_TC1); // ����жϱ�־
  }
}
/*********************************************************************
 * @fn      EXTI2_IRQHandler
 *
 * @brief   This function handles EXTI2 Handler.
 *
 * @return  none
 */
void EXTI7_0_IRQHandler(void)
{

  if (EXTI_GetITStatus(EXTI_Line2) != RESET)
  {

    // DEBUG_PRINT("have key msg\r\n"); // ����
    MOTOR_SET(1);
    Delay_Ms(100);
    MOTOR_SET(0);

    EXTI_ClearITPendingBit(EXTI_Line2); /* Clear Flag */

    if (!KEY0)
    {
      key.state = KEY_STATE_PRESS;
      DEBUG_PRINT("start press\r\n");
    }
    else
    {
      DEBUG_PRINT("end press\r\n");
      if (key.LongKeyCounter <= HOLD_TIME)
      {

        key.event = KEY_EVENT_CLICK;
        DEBUG_PRINT("KEY_EVENT_CLICK ontime\r\n");
      }
      else
      {
        key.event = KEY_EVENT_LONG_CLICK;
        DEBUG_PRINT("KEY_EVENT_LONG_CLICK ontime\r\n");
        key.LongKeyCounter = 0;
      }

      key.state = KEY_STATE_RELEASE;
    }
    key.LongKeyCounter = 0;

    if (key.enable == 0)
    {
      key.event = KEY_EVENT_NONE;
      key.state = KEY_STATE_IDLE;
      key.LongKeyCounter = 0;
      key.enable = 1;
      //  DEBUG_PRINT("disable key operate\r\n"); // ����Ϣ��������
    }
        system_wokeup();                    // ϵͳ����
  }

  if (EXTI_GetITStatus(EXTI_Line6) != RESET)
  {

    EXTI_ClearITPendingBit(EXTI_Line6); /* Clear Flag */
    loraComplete = 1;
    DEBUG_PRINT("lora msg\r\n");

    MOTOR_SET(1);
    Delay_Ms(100);
    MOTOR_SET(0);
    system_wokeup(); // ϵͳ����
  }

  if (EXTI_GetITStatus(EXTI_Line7) != RESET)
  {

    if (CHARGE)
    {
      charge.state = CHARGING;
      DEBUG_PRINT("start chage\r\n");
    }
    else
    {
      charge.state = UNCHARGING;
      DEBUG_PRINT("end chage\r\n");
    }

    MOTOR_SET(1);
    Delay_Ms(100);
    MOTOR_SET(0);
    system_wokeup();                    // ϵͳ����
    EXTI_ClearITPendingBit(EXTI_Line7); /* Clear Flag */
  }
}
/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{ // ���� NMI �쳣�Ĵ���
  // ���磬��¼������Ϣ�򴥷�ϵͳ��ȫ����
  DEBUG_PRINT("start NMI_Handler\r\n");
  while (1)
  {
  }
}

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
  // ���� HardFault �쳣�Ĵ���
  // ���磬��¼������Ϣ�����Իָ�ϵͳ������ϵͳ
  DEBUG_PRINT("start HardFault_Handler\r\n");
  while (1)
  {
  }
}

/*********************************************************************
 * @fn      EXTI0_INT_INIT
 *
 * @brief   Initializes EXTI0 collection.
 *
 * @return  none
 */
void EXTI_INT_INIT(void)
{
  EXTI_InitTypeDef EXTI_InitStructure = {0};
  NVIC_InitTypeDef NVIC_InitStructure = {0};

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  // GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource6|GPIO_PinSource2|GPIO_PinSource7);

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource6);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource7);

  EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line7;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  EXTI_InitStructure.EXTI_Line = EXTI_Line9;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = AWU_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

void system_wokeup()
{

  refresh_SleepCounter(0); // ˢ������ʱ��
 

  if (needSleep) // �Ѿ�������
  {
    //    My_GPIO_Init();                                                    // IO�ڳ�ʼ��****4484-4232=252�ֽ�
    TIM1_Init(100, (SystemCoreClock / (100 * PWM_FRE)) - 1, PWM_Duty); // ��Ļ�ı������  Ĭ�ϰٷְ�����******5076-4484=592�ֽ�pwmҪ200��+��ʱ��300
                                                                       //    TIM2_Init(11, 1);                                                  // ������������,����ֵΪ65535������Ƶ��1Ȧ12��****6020-6900=880�ֽ����벶��Ҫ500��+��ʱ��300
    LCD_Drive_Init();                                                  // ��ĻӲ����ʼ��****200�ֽ�
    Battery_Init();                                                    // ��ص�adc��ʼ��****9456-8636=820
                                                                       //  SX1278_Init();                                                     // ������Ҫ��ʼ��                                              // lora�ĳ�ʼ��*****10268-9620=648
                                                                       // EXTI_INT_INIT();                                                   // ��������磬lora�жϳ�ʼ��
    USART_Printf_Init(115200);

    DEBUG_PRINT("system_wokeup\r\n");

      // �������¼�������¼�
  key.event = KEY_EVENT_NONE;
  encode.state = ENCODE_EVENT_NONE;
     needSleep = 0;
  }
}

void system_enter_sleep()
{

  if (needDeinit)
  {
    // My_GPIO_DeInit();

    LCD_Drive_DeInit();
    Battery_DeInit();

    TIM1_DeInit();
    //   TIM2_DeInit();
    needDeinit = 0;

    DEBUG_PRINT("system_Deinit\r\n");
  }
}

void Sleep_Scan()
{
  if (needSleep)
  {
    system_enter_sleep();
    PWR_EnterSTANDBYMode(PWR_STANDBYEntry_WFI);
  }
}

// ��ʱ���жϷ�����
void TIM1_UP_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
  {
    // ����жϱ�־
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    SleepCounter++;
    if (SleepCounter >= 50000) // 15s����һ��
    {

      DEBUG_PRINT("EnterSTANDBYMode\r\n");

      SleepCounter = 0;

      needSleep = 1;
      needDeinit = 1;
    }

    if (!KEY0)
    {
      key.LongKeyCounter++;
      key.state = KEY_STATE_HOLD;
    }
  }
}
void AWU_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      EXTI0_IRQHandler
 *
 * @brief   This function handles EXTI0 Handler.
 *
 * @return  none
 */
void AWU_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line9) != RESET)
  {
    printf("AWU Wake_up\r\n");
    EXTI_ClearITPendingBit(EXTI_Line9); /* Clear Flag */
  }
}
