/*
 * page.h
 *
 *  Created on: 2024年9月21日
 *      Author: 12630
 */

#ifndef SCREEN_PAGE_H_
#define SCREEN_PAGE_H_
#include "debug.h"
#include "encode.h"
#include "adc.h"
#include "seting.h"

// #include "SX1278.h"

extern Key key;

extern Encode encode_struct;
extern volatile u8 lora_receive_len;
extern volatile u8 lora_receive_flag;

extern char lora_receive_buf[145];
extern u16 Battery_ADC_Average;

#define ON 1
#define OFF 0

// Lora参数范围定义
#define LORAFREQ_MIN 1
#define LORAFREQ_MAX 11
#define LORAPOWER_MIN 11
#define LORAPOWER_MAX 20
#define LORASpreadFactor_MIN 7
#define LORASpreadFactor_MAX 12
#define LORABANDWIDTH_MIN 0
#define LORABANDWIDTH_MAX 9

// 定义设置参数
typedef enum
{
  SETTING_SCREEN_LIGHT,
  SETTING_SHAKE_MODE,
  SETTING_LORA_FREQ,
  SETTING_LORA_POWER,
  SETTING_LORA_BANDWIDTH,
  SETTING_LORA_SPREAD_FACTOR,
  SETTING_COUNT
} SettingIndex;

extern u8 shake_mode;        // 振动模式，0为off，1为on
extern u8 Lora_Freq;         // 默认频率设置
extern u8 Lora_Power;        // 输出功率设置
extern u8 Lora_BandWide;     // 带宽设置
extern u8 Lora_SpreadFactor; // 扩频因子设置在7~12= 7

typedef struct
{
  const char *name;
  u8 *value;
  void (*update_func)(void);
} Setting;
// 定义按键事件
typedef enum
{
  PAGE_SEND, // 发送页面
  PAGE_PERPARE_SETTING, // 准备设置页面
  PAGE_HISTROY_CHAT,

  PAGE_SETTING, // 设置页面
  PAGE_INFO,    // 信息页面
} Page;

void chat_page(sFONT *Font);
void show_history_data(sFONT *Font);
void chat_history_page(sFONT *Font);
void perpare_setting_page(sFONT *Font);
void setting_page(sFONT *Font);
void show_page();
void info_page();
#endif /* SCREEN_PAGE_H_ */
