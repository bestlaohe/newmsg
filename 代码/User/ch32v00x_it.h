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
#include "SX1278.h"
#include "time_app.h"
#include "drive_screen.h"
#include "seting.h"
extern u16 BattaryBuf[ADC_CONUT];
// 定义按键状态
typedef enum
{
  KEY_STATE_IDLE,   // 空闲状态
  KEY_STATE_PRESS,  // 按下状态
  KEY_STATE_HOLD,   // 长按状态
  KEY_STATE_RELEASE // 松开状态
} KeyState;

// 定义按键事件
typedef enum
{
  KEY_EVENT_NONE,       // 无事件
  KEY_EVENT_CLICK,      // 单机事件
  KEY_EVENT_LONG_CLICK, // 长按事件
} KeyEvent;

// 定义按键结构体
typedef struct
{
  KeyState state;          // 当前按键状态
  KeyEvent event;          // 当前按键事件
  u8 debounce_counter;     // 去抖动计数器
  uint16_t LongKeyCounter; // 长按计数器
  u8 enable;               // 使能

} Key;

// 定义按键事件
typedef enum
{
  ENCODE_EVENT_NONE, // 无事件
  ENCODE_EVENT_UP,   // 向上滚动
  ENCODE_EVENT_DOWN, // 向下滚动
} EncodeState;

// 定义按键结构体
typedef struct
{
  EncodeState state; // 当前按键状态
 u8 enable;               // 使能
} Encode;


// 定义按键事件
typedef enum
{
  UNCHARGING,   // 没充电
  CHARGING, // 充电中

} ChargeState;

// 定义按键结构体
typedef struct
{
  ChargeState state; // 当前按键状态

} Charge;

extern volatile u8 needSleep;
extern volatile u8 needDeinit;//改成u8就无法正常休眠


void process_motor_flags(void);
void refresh_SleepCounter(int newtime);
void EXTI_INT_INIT(void);
void Sleep_Scan();
#endif /* __CH32V00x_IT_H */
