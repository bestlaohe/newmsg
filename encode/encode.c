/*
 * encode.c
 *
 *  Created on: 2024年9月1日
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
#include "SX1278.h"

void Encoder_Init(u16 arr, u16 psc)
{
    TIM2_Init(arr, psc); // 自动重装载值（ARR）和预分频器（PSC）
}
/*********************************************************************
 * @fn      KEY_PRESS
 *
 * @brief   Key processing funcation.
 *
 * @return  0 - Press the key.（pd2）
 *          1 - Release Key.
 */
u8 KEY_PRESS(void)
{
    if (KEY0 == 1)
    {
        Delay_Ms(10);
        if (KEY0 == 1)
            return 1;
        else
            return 0;
    }

    return 0;
}

void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

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
        printf("按键中断触发震动\r\n"); // 有消息发来就震动
        MOTOR_ON;
        Delay_Ms(100);
        MOTOR_OFF;
        system_wokeup();                    // 系统唤醒
        EXTI_ClearITPendingBit(EXTI_Line2); /* Clear Flag */
    }

    if (EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
        printf("我lora来数据中断触发震动\r\n"); // 有消息发来就震动
        MOTOR_ON;
        Delay_Ms(100);
        MOTOR_OFF;
        system_wokeup();                    // 系统唤醒
        EXTI_ClearITPendingBit(EXTI_Line6); /* Clear Flag */

    }
}

/*********************************************************************
 * @fn      EXTI0_INT_INIT
 *
 * @brief   Initializes EXTI0 collection.
 *
 * @return  none
 */
void KEY_INIT(void)
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2);
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
