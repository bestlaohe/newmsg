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
#include "seting.h"
// #include <core_riscv.h>
void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast"))); // 作为倒计时10ms一次触发中断
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void AWU_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

volatile u8 dmaTransferComplete = 0;
volatile u8 loraComplete = 0;
volatile u8 needSleep = 0;
volatile u8 needDeinit = 0;

u8 motor_shaking = 0;
u16 motor_shake_time = 0;

volatile int circle = 0;
int SleepCounter = 0;

// 添加标志位用于延时操作
volatile u8 needMotorShakeKey = 0;
volatile u8 needMotorShakeLora = 0;
volatile u8 needMotorShakeCharge = 0;
volatile u8 needMotorShakeEncode = 0;

Encode encode_struct = {ENCODE_EVENT_NONE, 0};
Key key = {KEY_STATE_IDLE, KEY_EVENT_NONE, 0, 0, 1};
Charge charge = {UNCHARGING};

void TIM2_IRQHandler()
{

  volatile uint16_t tempcnt = TIM2->CNT, temparr = TIM2->ATRLR;

  if (TIM_GetITStatus(TIM2, TIM_IT_Update))
  {

    //     DEBUG_PRINT("当前计数=%d\r\n", tempcnt);
    //     DEBUG_PRINT("重装载值=%d\r\n", temparr);

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
  // DEBUG_PRINT("refresh_SleepCounter\r\n");
}

void DMA1_Channel3_IRQHandler(void)
{
  if (DMA_GetITStatus(DMA1_IT_TC3))
  {
    //   DEBUG_PRINT("dmaTransferComplete\r\n");
    dmaTransferComplete = 1;
    DMA_ClearITPendingBit(DMA1_IT_TC3); // 清除中断标志
  }
}
u16 Battery_ADC_Average = 0;
u16 pre_Battery_ADC_Average = 0;
void DMA1_Channel1_IRQHandler(void)
{

  if (DMA_GetITStatus(DMA1_IT_TC1))
  {

    Battery_ADC_Average = 0;
    // 计算电池平均值
    for (u8 i = 0; i < ADC_CONUT; i++)
    {
      Battery_ADC_Average += BattaryBuf[i];
      //        DEBUG_PRINT("BattaryBuf[i]=%d\r\n",BattaryBuf[i]);
    }
    Battery_ADC_Average /= ADC_CONUT; // 求平均值

    if (abs(pre_Battery_ADC_Average - Battery_ADC_Average) < 10)
    {
      Battery_ADC_Average = pre_Battery_ADC_Average;
    }
    else
    {
      pre_Battery_ADC_Average = Battery_ADC_Average;
    }
    //  DEBUG_PRINT("Battery_ADC_Average=%d\r\n",Battery_ADC_Average);

    DMA_ClearITPendingBit(DMA1_IT_TC1); // 清除中断标志
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

    needMotorShakeKey = 1;

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
        if (key.enable == 0)
        {
          key.event = KEY_EVENT_NONE;
          key.state = KEY_STATE_IDLE;
          key.LongKeyCounter = 0;
          key.enable = 1;
          DEBUG_PRINT("disable key operate\r\n");
        }
      }
      if (key.enable == 0)
      {
        key.event = KEY_EVENT_NONE;
        key.state = KEY_STATE_IDLE;
        key.LongKeyCounter = 0;
        key.enable = 1;
        DEBUG_PRINT("disable key operate\r\n");
      }
      key.state = KEY_STATE_RELEASE;
    }
    key.LongKeyCounter = 0;

    system_wokeup(); // 系统唤醒
  }

  if (EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line6); /* Clear Flag */
    loraComplete = 1;
    DEBUG_PRINT("lora operate\r\n"); // 不管发送还是接收都会触发

    needMotorShakeLora = 1;

    system_wokeup(); // 系统唤醒
  }

  if (EXTI_GetITStatus(EXTI_Line1) != RESET)
  {

    needMotorShakeCharge = 1;

    if (!CHARGE)
    {
      charge.state = CHARGING;
      DEBUG_PRINT("start chage\r\n");
    }
    else
    {
      charge.state = UNCHARGING;
      DEBUG_PRINT("end chage\r\n");
    }

    system_wokeup();                    // 系统唤醒
    EXTI_ClearITPendingBit(EXTI_Line1); /* Clear Flag */
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
// 这里我们不是正常的中断,而是hardfault,所以折了的mcause对应的是Load address misaligned
// Interrupt	Exception Code	Description
// 1	0	User software interrupt
// 1	1	Supervisor software interrupt
// 1	2	Hypervisor software interrupt
// 1	3	Machine software interrupt
// 1	4	User timer interrupt
// 1	5	Supervisor timer interrupt
// 1	6	Hypervisor timer interrupt
// 1	7	Machine timer interrupt
// 1	8	User external interrupt
// 1	9	Supervisor external interrupt
// 1	10	Hypervisor external interrupt
// 1	11	Machine external interrupt
// 1	≥12	Reserved
// 0	0	Instruction address misaligned
// 0	1	Instruction access fault
// 0	2	Illegal instruction
// 0	3	Breakpoint
// 0	4	Load address misaligned
// 0	5	Load access fault
// 0	6	Store/AMO address misaligned
// 0	7	Store/AMO access fault
// 0	8	Environment call from U-mode
// 0	9	Environment call from S-mode
// 0	10	Environment call from H-mode
// 0	11	Environment call from M-mode
// 0	≥12	Reserved

// 变量地址mtval
void HardFault_Handler(void)
{
  // 处理 HardFault 异常的代码
  // 例如，记录故障信息、尝试恢复系统或重启系统
  DEBUG_PRINT("start HardFault_Handler\r\n");

  //  uint32_t v_mepc, v_mcause;
  //
  //  v_mepc = __get_MEPC();
  //  v_mcause = __get_MCAUSE();
  //
  //
  //  printf("mepc:%08x\n", v_mepc);
  //  printf("mcause:%08x\n", v_mcause);

  while (1)
    ;
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

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource6);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2);

  EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line1; // key和charge
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  EXTI_InitStructure.EXTI_Line = EXTI_Line6; // lora
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  EXTI_InitStructure.EXTI_Line = EXTI_Line9; // AWU
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

  refresh_SleepCounter(0); // 刷新休眠时间

  if (needSleep) // 已经休眠了
  {
    //    My_GPIO_Init();                                                    // IO口初始化****4484-4232=252字节
    TIM1_Init(100, (SystemCoreClock / (100 * PWM_FRE)) - 1, PWM_Duty); // 屏幕的背光调节  默认百分百亮度******5076-4484=592字节pwm要200多+定时器300

#if ENCODER_ENABLED
      TIM2_Init(11, 1);                                                  // 编码器的内容,重载值为65535，不分频，1圈12个****6020-6900=880字节输入捕获要500多+定时器300
#endif

#if SCREEN_ENABLED
    LCD_Drive_Init(); // 屏幕硬件初始化****200字节
#endif

#if BATTERY_ENABLED
    Battery_Init(); // 电池的adc初始化****9456-8636=820
#endif

#if LORA_ENABLED
    //  SX1278_Init();                                                     // 可能需要初始化                                              // lora的初始化*****10268-9620=648
#endif

    //  EXTI_INT_INIT();                                                   // 按键，充电，lora中断初始化
    USART_Printf_Init(115200);

    DEBUG_PRINT("system_wokeup\r\n");

    // 处理完事件后清除事件
    key.event = KEY_EVENT_NONE;
    encode_struct.state = ENCODE_EVENT_NONE;
    needSleep = 0;
  }
}

void system_enter_sleep()
{

  if (needDeinit)
  {
    DEBUG_PRINT("system_Deinit\r\n");
    // My_GPIO_DeInit();//唤醒不了打开的话

#if LORA_ENABLED

    SX1278_Sleep();

    //  SX1278_Standby();
#endif

#if SCREEN_ENABLED
    LCD_Drive_DeInit();
#endif

#if BATTERY_ENABLED
    Battery_DeInit();
#endif

    TIM1_DeInit();

#if ENCODER_ENABLED
    TIM2_DeInit(); // 编码器用
#endif

    USART_DeInit(USART1);
    needDeinit = 0;
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

// 定时器中断服务函数10ms  100us
void TIM1_UP_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
  {
    // 清除中断标志
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

#if SLEEP == 1
    SleepCounter++;
    if (SleepCounter >= SLEEP_TIME)
    {
      SleepCounter = 0;
      needSleep = 1;
      needDeinit = 1;
      DEBUG_PRINT("EnterSTANDBYMode\r\n");
    }
#endif

    if (!KEY0)
    {
      key.LongKeyCounter++;
      if (key.LongKeyCounter >= DEBOUNCE_TIME) // 消抖
        key.state = KEY_STATE_HOLD;
    }

    if (motor_shaking)
    {
      motor_shake_time++;
      if (motor_shake_time >= SHAKE_TIME) // 消抖
      {

        DEBUG_PRINT("endshake\r\n");
        motor_shaking = 0;
        motor_shake_time = 0;
        MOTOR_SET(0);
      }
    }
  }
}

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

#if SLEEP == 1
    DEBUG_PRINT("AWU Wake_up=%d\r\n", SleepCounter);
#endif
    EXTI_ClearITPendingBit(EXTI_Line9); /* Clear Flag */
  }
}

void process_motor_flags(void)
{
  // 只要有任何一个震动标志位被设置，就执行一次震动
  if (needMotorShakeKey || needMotorShakeLora || needMotorShakeCharge || needMotorShakeEncode)
  {
    MOTOR_SET(1);

    // 清除所有标志位
    needMotorShakeKey = 0;
    needMotorShakeLora = 0;
    needMotorShakeCharge = 0;
    needMotorShakeEncode = 0;
    motor_shaking = 1;
    motor_shake_time = 0;
  }
}
