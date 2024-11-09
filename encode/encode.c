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
#include "SX1278.h"

uint16_t precnt = 0;
int precircle = 0;
void Encoder_Scan()
{

  if (precircle != circle || (precnt != TIM2->CNT)) // �б仯�Ͷ�   140�ֽ�
  {
    if (precircle * 12 + precnt > circle * 12 + TIM2->CNT)
    {
      encode.state = ENCODE_EVENT_DOWN;
      // DEBUG_PRINT("����");
    }
    encode.state = ENCODE_EVENT_UP;
    if (precircle * 12 + precnt < circle * 12 + TIM2->CNT)
    {
      encode.state = ENCODE_EVENT_DOWN;
      // DEBUG_PRINT("����");
    }

    // DEBUG_PRINT("precircle*12+precnt= %d circle*12+TIM2->CNT %d step\r\n", precircle * 12 + precnt, circle * 12 + TIM2->CNT);
    // DEBUG_PRINT("Encoder position= %d circle %d step\r\n", TIM2->CNT, circle);
    precircle = circle;
    precnt = TIM2->CNT;

     refresh_SleepCounter(0); // ˢ������ʱ��
    MOTOR_SET(1);
    Delay_Ms(50);
    MOTOR_SET(0);
  }
}
void Encoder_Init(u16 arr, u16 psc)
{
  TIM2_Init(arr, psc); // �Զ���װ��ֵ��ARR����Ԥ��Ƶ����PSC��
}
/*********************************************************************
 * @fn      KEY_PRESS
 *
 * @brief   Key processing funcation.
 *
 * @return  0 - Press the key.��pd2��
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
