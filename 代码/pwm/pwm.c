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



void PWM_Config(u16 Freq, u16 Duty)//(10000, 100);
{
    // 确保频率在合理范围内
    if (Freq == 0) return; // 避免除以零
    if (Duty > 100) Duty = 100; // 确保占空比不超过100%

    // 计算预分频器和自动重载寄存器值
    u16 psc = (SystemCoreClock / (100*Freq)) - 1; // 计算预分频器24

    // 调用初始化函数
    TIM1_Init(100, psc, Duty);
}



