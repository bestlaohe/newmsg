/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v00x_it.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/08
 * Description        : This file contains the headers of the interrupt handlers.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32V00x_IT_H
#define __CH32V00x_IT_H

#include "debug.h"
#include "gpio.h"
// ���尴��״̬
typedef enum
{
  KEY_STATE_IDLE,   // ����״̬
  KEY_STATE_PRESS,  // ����״̬
  KEY_STATE_HOLD,   // ����״̬
  KEY_STATE_RELEASE // �ɿ�״̬
} KeyState;

// ���尴���¼�
typedef enum
{
  KEY_EVENT_NONE,   // ���¼�
  KEY_EVENT_CLICK,  // �����¼�
  KEY_EVENT_LONG_CLICK,   // �����¼�
} KeyEvent;

// ���尴���ṹ��
typedef struct
{
  KeyState state;            // ��ǰ����״̬
  KeyEvent event;            // ��ǰ�����¼�
  uint16_t debounce_counter; // ȥ����������
  uint16_t LongKeyCounter;   // ����������
  u8 enable;   // ʹ��

} Key;


// ���尴���¼�
typedef enum
{
    ENCODE_EVENT_NONE,   // ���¼�
  ENCODE_EVENT_UP,  // �����¼�
  ENCODE_EVENT_DOWN,   // �����¼�
} EncodeState;

// ���尴���ṹ��
typedef struct
{
  EncodeState state;            // ��ǰ����״̬

} Encode;



void refresh_SleepCounter(int newtime);

#endif /* __CH32V00x_IT_H */


