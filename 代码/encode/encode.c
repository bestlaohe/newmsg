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
extern volatile u8 needMotorShakeEncode;
void Encoder_Scan()
{

  static u8 precnt = 0;
  static int precircle = 0;
  if (precnt == 0 && precircle == 0)
  {

    precnt = TIM2->CNT;
    precircle = circle;
  }
  if (precircle != circle || (precnt != TIM2->CNT)) // 有变化就动   140字节
  {
    if (precircle * 12 + precnt > circle * 12 + TIM2->CNT)
    {
      encode_struct.state = ENCODE_EVENT_UP;
      DEBUG_PRINT("Encoder up\r\n");
    }
 
    if (precircle * 12 + precnt < circle * 12 + TIM2->CNT)
    {
      encode_struct.state = ENCODE_EVENT_DOWN;
      DEBUG_PRINT("Encoder down\r\n");
    }

    // DEBUG_PRINT("precircle*12+precnt= %d circle*12+TIM2->CNT %d step\r\n", precircle * 12 + precnt, circle * 12 + TIM2->CNT);
    // DEBUG_PRINT("Encoder position= %d circle %d step\r\n", TIM2->CNT, circle);
    // DEBUG_PRINT("precircle= %d precnt= %d \r\n", precircle, precnt);
    precircle = circle;
    precnt = TIM2->CNT;
    if (encode_struct.enable == 0)
    {
      encode_struct.enable = 1;
      encode_struct.state = ENCODE_EVENT_NONE;
      DEBUG_PRINT("disable encode_struct operate\r\n");
    }
    refresh_SleepCounter(0); // 刷新休眠时间
  needMotorShakeEncode=1;
  }
}
void Encoder_Init(u16 arr, u16 psc)
{
  TIM2_Init(arr, psc); // 自动重装载值（ARR）和预分频器（PSC）
}
