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

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast"))); // ��Ϊ����ʱ10msһ�δ����ж�
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

volatile int dmaTransferComplete = 0;

volatile int circle = 0;
int SleepCounter = 0;
char lora_receive_buf[50] = {0};

Encode encode = {ENCODE_EVENT_NONE};
Key key = {KEY_STATE_IDLE, KEY_EVENT_NONE, 0, 0, 1};
// ȥ�����ͳ������ĳ���
#define DEBOUNCE_TIME 50 // ȥ����ʱ�䣬��λ��ms
#define HOLD_TIME 3000   // ����ʱ�䣬��λ��100us
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
void system_wokeup()
{

  refresh_SleepCounter(0); // ˢ������ʱ��
}

// ��ʱ���жϷ�����
void TIM1_UP_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
  {

    SleepCounter++;
    if (SleepCounter >= 150000) // 15s����һ��
    {
      DEBUG_PRINT("EnterSTANDBYMode\r\n");
      SleepCounter = 0;
      // PWR_EnterSTANDBYMode(PWR_STANDBYEntry_WFI);
    }

    if (!KEY0)
    {
      key.LongKeyCounter++;
      key.state = KEY_STATE_HOLD;
    }

    // ����жϱ�־
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
  }
}

void DMA1_Channel3_IRQHandler(void)
{
  if (DMA_GetITStatus(DMA1_IT_TC3))
  {
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
    system_wokeup();                    // ϵͳ����
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
  }

  if (EXTI_GetITStatus(EXTI_Line6) != RESET)
  {

 EXTI_ClearITPendingBit(EXTI_Line6); /* Clear Flag */
// SX1278_LoRaEntryRx(); // �������ģʽ
//    u8 packet_length;
//
//    u8 addr, irq_flag;
//    u8 packet_size;
//
//    irq_flag = SX1278_Read_Reg(LR_RegIrqFlags);
//
//    if ((irq_flag & RFLR_IRQFLAGS_PAYLOADCRCERROR) == RFLR_IRQFLAGS_PAYLOADCRCERROR) // �����CRCУ������ж�
//    {
//      SX1278_LoRaClearIrq();
//      SX1278_Sleep(); // ����˯��ģʽ��Ϊconfig��׼��
//      DEBUG_PRINT(" lora crc error ");
//     // return;
//    }
//
//    addr = SX1278_Read_Reg(LR_RegFifoRxCurrentaddr); // last packet addr
//    DEBUG_PRINT("addr  %d\r\n", addr);
//    SX1278_Write_Reg(LR_RegFifoAddrPtr, addr);      // RxBaseAddr -> FiFoAddrPtr
//    packet_size = SX1278_Read_Reg(LR_RegRxNbBytes); // Number for received bytes
//    SX1278_Burst_Read(0x00, lora_receive_buf, packet_size);
//    packet_length = packet_size;
//
//    SX1278_LoRaClearIrq();
//    SX1278_Sleep(); // ����˯��ģʽ��
//
//    DEBUG_PRINT("have lora msg %d\r\n", packet_length); // ����Ϣ��������
//
    MOTOR_SET(1);
    Delay_Ms(100);
    MOTOR_SET(0);
    system_wokeup(); // ϵͳ����

   
  }

  if (EXTI_GetITStatus(EXTI_Line7) != RESET)
  {

    if (CHARGING)
    {

      DEBUG_PRINT("start chage\r\n");
    }

    else
    {

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

  EXTI_InitStructure.EXTI_Line = EXTI_Line6 | EXTI_Line2 | EXTI_Line7;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
