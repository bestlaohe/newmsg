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
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast"))); // 作为倒计时10ms一次触发中断
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

volatile int circle = 0;
int SleepCounter = 0;
int dmacircular = 0;
int LongKeyCounter = 0;


Key key = {KEY_STATE_IDLE, KEY_EVENT_NONE, 0, 0};
// 去抖动和长按检测的常数
#define DEBOUNCE_TIME 50 // 去抖动时间，单位：ms
#define HOLD_TIME 5000   // 长按时间，单位：100us
void TIM2_IRQHandler()
{

  volatile uint16_t tempcnt = TIM2->CNT, temparr = TIM2->ATRLR;

  if (TIM_GetITStatus(TIM2, TIM_IT_Update))
  {
    // printf("当前计数=%d\r\n", tempcnt);
    // printf("重装载值=%d\r\n", temparr);

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

  refresh_SleepCounter(0); // 刷新休眠时间
}

// 定时器中断服务函数
void TIM1_UP_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
  {

    SleepCounter++;
    if (SleepCounter >= 150000) // 15s触发一次
    {
      printf("EnterSTANDBYMode\r\n");
      SleepCounter = 0;
      // PWR_EnterSTANDBYMode(PWR_STANDBYEntry_WFI);
    }

    if (!KEY0)
      key.LongKeyCounter++;
    if (key.LongKeyCounter >= HOLD_TIME) // 500ms触发一次
    {
      key.state = KEY_STATE_HOLD;
      printf("longkey ontime\r\n");
      key.LongKeyCounter = 0;
    }

    // 清除中断标志
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
  }
}

void DMA1_Channel3_IRQHandler(void)
{

  if (DMA_GetITStatus(DMA1_IT_TC3))
  {
    // 传输完成处理
    //  printf("一开始DMA传输完成%d\r\n", dmacircular);
    // 清除中断标志
    DMA_ClearITPendingBit(DMA1_IT_TC3);

    // printf("CFGR%x\r\n", DMA1_Channel3->CFGR);

    //  printf("DMA_Mode_Circular%x\r\n", DMA_Mode_Circular);

    // printf("结果%x\r\n", DMA1_Channel3->CFGR & 0x00b0);

    if ((DMA1_Channel3->CFGR & 0x00b0) == 0x00b0) // 0x00b0是循环，92是正常
      dmacircular++;

    if ((DMA1_Channel3->CFGR & 0x00b0) == 0x00b0 && dmacircular == 29)
    {
      dmacircular = 0;
      DMA_Cmd(DMA1_Channel3, DISABLE);
      SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
      DMA_ClearITPendingBit(DMA1_IT_TC3);

      LCD_CS_1;
    }
    Delay_Ms(1);
    // printf("结束DMA传输完成%d\r\n", dmacircular);
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

    printf("have key msg\r\n"); // 按键
    MOTOR_ON;
    Delay_Ms(100);
    MOTOR_OFF;
    system_wokeup();                    // 系统唤醒
    EXTI_ClearITPendingBit(EXTI_Line2); /* Clear Flag */
    key.LongKeyCounter = 0;

    if (!KEY0)
    {
      key.state = KEY_STATE_PRESS;
      printf("start press\r\n");
    }
    else
    {
      printf("end press\r\n");
      key.state = KEY_STATE_RELEASE;
    }
  }

  if (EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
    printf("have lora msg\r\n"); // 有消息发来就震动
    MOTOR_ON;
    Delay_Ms(100);
    MOTOR_OFF;
    system_wokeup();                    // 系统唤醒
    EXTI_ClearITPendingBit(EXTI_Line6); /* Clear Flag */
  }
  if (EXTI_GetITStatus(EXTI_Line7) != RESET)
  {

    if (CHARGING)
      printf("start chage\r\n");
    else
      printf("end chage\r\n");

    MOTOR_ON;
    Delay_Ms(100);
    MOTOR_OFF;
    system_wokeup();                    // 系统唤醒
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
{ // 处理 NMI 异常的代码
  // 例如，记录错误信息或触发系统安全机制
  printf("start NMI_Handler\r\n");
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
  // 处理 HardFault 异常的代码
  // 例如，记录故障信息、尝试恢复系统或重启系统
  printf("start HardFault_Handler\r\n");
  while (1)
  {
  }
}
