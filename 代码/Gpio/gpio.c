/*
 * gpio.c
 *
 *  Created on: 2024年9月1日
 *      Author: 12630
 */

#include "gpio.h"
void GPIO_Init_Pin(GPIO_TypeDef *GPIOx, uint16_t pin, GPIOMode_TypeDef mode)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Mode = mode;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz; // 假设大多数情况下速度都一样
    GPIO_Init(GPIOx, &GPIO_InitStructure);
}

void My_GPIO_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    // 充电检测
    GPIO_Init_Pin(GPIOC, GPIO_Pin_1, GPIO_Mode_IPD);
    // 屏幕的复位
    GPIO_Init_Pin(GPIOD, GPIO_Pin_7, GPIO_Mode_Out_PP);
    // pc0的lora的nss
    GPIO_Init_Pin(GPIOD, GPIO_Pin_0, GPIO_Mode_Out_PP);
    // pc1的lora的复位
    GPIO_Init_Pin(GPIOC, GPIO_Pin_0, GPIO_Mode_Out_PP);

    // pd2的确认按钮
    GPIO_Init_Pin(GPIOD, GPIO_Pin_2, GPIO_Mode_IPU);
    // pd6的lora的中断
    GPIO_Init_Pin(GPIOD, GPIO_Pin_6, GPIO_Mode_IPD);
    // 和lora通信miso
    GPIO_Init_Pin(GPIOC, GPIO_Pin_7, GPIO_Mode_IN_FLOATING);

    // pA2震动模块的
    GPIO_Init_Pin(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP);
    // pA1电池电量采集
    GPIO_Init_Pin(GPIOA, GPIO_Pin_1, GPIO_Mode_AIN);
    // pc3调光的pwm
    GPIO_Init_Pin(GPIOC, GPIO_Pin_3, GPIO_Mode_AF_PP);
    // 复用推挽输出模式，LCD_DC_1   LCD_CS
    GPIO_Init_Pin(GPIOC, GPIO_Pin_2 | GPIO_Pin_4, GPIO_Mode_Out_PP);

    // 编码器的io口d3d4
    GPIO_Init_Pin(GPIOD, GPIO_Pin_3 | GPIO_Pin_4, GPIO_Mode_IPU);
    // LCD_SCK_1;  SPI_SDA_1;
    GPIO_Init_Pin(GPIOC, GPIO_Pin_5 | GPIO_Pin_6, GPIO_Mode_AF_PP);

    MOTOR_OFF; // 关闭电机
    LCD_CS_DISABLE;
    SX1278_NSS_DISABLE;
}
void My_GPIO_DeInit()
{
    GPIO_DeInit(GPIOA);
    GPIO_DeInit(GPIOC);
    GPIO_DeInit(GPIOD);
}

u8 shake_mode = 1; // 初始震动模式值
void MOTOR_SET(int state)
{
    // DEBUG_PRINT("MOTOR_SET=%d,%d\r\n", shake_mode, state);
    if (shake_mode)
    {
        if (state)
        {
            MOTOR_ON;
        }else
            MOTOR_OFF;
    }
}
