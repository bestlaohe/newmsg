/*
 * page.c
 *
 *  Created on: 2024年9月21日
 *      Author: 12630
 */
#include "page.h"
#include <string.h>
int8_t Englishcount = 0; // 字符的位号
int Englishposx = 0;     // 字符的位置
int Englishposy = 0;     // 字符的位置
static u8 lora_send_buf[50];

void chat_page(sFONT *Font)
{
  // 显示电池信息
  show_battery();

  // 绘制聊天页面的基本组件
  Paint_DrawChar(0, 0, 0, &Font16_Operate, BLACK, BLUE, 0);

  Paint_DrawRectangle(0, 20, 127, 100, RED, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

  Paint_DrawRectangle(0, 102, 127, 127, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

  // 绘制字符
  Paint_DrawChar(2 + Englishposx * Font->Width, 103 + Englishposy * Font->Height, 'a' + Englishcount, Font, BLACK, WHITE, 'a');
  DEBUG_PRINT("chat_page5\r\n");

  // 处理滚动状态
  if (encode.state == ENCODE_EVENT_UP)
  {
    Englishcount = (Englishcount + 1) % 26; // 循环计数
  }
  else if (encode.state == ENCODE_EVENT_DOWN)
  {
    Englishcount = (Englishcount - 1 + 26) % 26; // 循环计数
  }

  // 发送数据
  if (key.state == KEY_STATE_HOLD)
  {
    if (encode.state == ENCODE_EVENT_UP)
    {
      // 发送数据
      if (!SX1278_LoRaTxPacket(lora_send_buf, Englishposx + Englishposy * (LCD_HEIGHT - 3 / Font->Width)))
      {
        DEBUG_PRINT("lora send ok \r\n");
      }
      key.enable = 0; // 禁用键
    }
    else if (encode.state == ENCODE_EVENT_DOWN)
    {
      // 输入回退
      if (--Englishposx < 0)
      {
        if (Englishposy > 0)
        {
          Englishposy--;
          Englishposx = (LCD_WIDTH - 2) / Font->Width - 1; // 重置列位置
        }
        else
        {
          Englishposx = 0;
        }
      }
      key.enable = 0; // 禁用键
    }
  }

  // 确认输入
  if (key.event == KEY_EVENT_CLICK)
  {
    lora_send_buf[Englishposx + Englishposy * (LCD_HEIGHT - 3 / Font->Width)] = 'a' + Englishcount;
    Englishposx++;

    if (2 + Englishposx * Font->Width >= LCD_WIDTH - 1)
    {
      Englishposx = 0;
      Englishposy++;
      if (103 + Englishposy * Font->Height >= LCD_HEIGHT - 1)
      {
        Englishposy = 0; // 重置行位置
      }
    }
    Englishcount = 0; // 重置计数
  }
}

void chat_history_page()
{
  Paint_DrawRectangle(0, 20, 127, 100, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawRectangle(0, 102, 127, 127, BLUE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  show_history_data();
  if (encode.state == ENCODE_EVENT_UP) // 滚动
  {
  }

  if (encode.state == ENCODE_EVENT_DOWN) // 滚动
  {
  }
}
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

uint32_t volatile *screen_light = &TIM1->CH3CVR; // 初始屏幕亮度值
extern u8 shake_mode;                            // 振动模式，0为off，1为on
extern u8 Lora_Freq;                             // 默认频率设置
extern u8 Lora_Power;                            // 输出功率设置
extern u8 Lora_BandWide;                         // 带宽设置
extern u8 Lora_SpreadFactor;                     // 扩频因子设置在7~12= 7

typedef struct
{
  const char *name;
  u8 *value;
  void (*update_func)(void);
} Setting;

void update_Lora(void)
{
  SX1278_Init();
}

// 定义设置数组
Setting settings[SETTING_COUNT] = {
    {"屏幕亮度:", NULL, NULL},
    {"振动模式:", &shake_mode, NULL},
    {"Lora频率:", &Lora_Freq, update_Lora},
    {"Lora功率:", &Lora_Power, update_Lora},
    {"Lora带宽:", &Lora_BandWide, update_Lora}, // 根据需要定义带宽的值
    {"Lora扩频因子:", &Lora_SpreadFactor, update_Lora}};

int current_setting = 0;

void clamp_value(u8 *value, u8 min, u8 max)
{
  if (*value < min)
    *value = min;
  if (*value > max)
    *value = max;
}

void update_current_setting(int value)
{
  if (current_setting == SETTING_SCREEN_LIGHT)
  {
    // 限制亮度在0到100之间
    value = (value < 0) ? 0 : (value > 100) ? 100
                                            : value;
    *screen_light = value;
  }
  else if (current_setting == SETTING_SHAKE_MODE)
  {
    // 切换振动模式
    *settings[current_setting].value = !(*settings[current_setting].value);
  }
  else if (current_setting == SETTING_LORA_FREQ)
  {
    clamp_value(&Lora_Freq, LORAFREQ_MIN, LORAFREQ_MAX);
  }
  else if (current_setting == SETTING_LORA_POWER)
  {
    clamp_value(&Lora_Power, LORAPOWER_MIN, LORAPOWER_MAX);
  }
  else if (current_setting == SETTING_LORA_BANDWIDTH)
  {
    clamp_value(&Lora_BandWide, LORABANDWIDTH_MIN, LORABANDWIDTH_MAX);
  }
  else if (current_setting == SETTING_LORA_SPREAD_FACTOR)
  {
    clamp_value(&Lora_SpreadFactor, LORASpreadFactor_MIN, LORASpreadFactor_MAX);
  }
  if (settings[current_setting].update_func)
  {
    settings[current_setting].update_func();
  }
}

void draw_setting(int index, int highlight)
{
  char strBuf[5]; // 用于存储最多3位数字和一个终止符
  UWORD bg_color = highlight ? GREEN : WHITE;

  Paint_DrawString(0, index * 10, settings[index].name, &Font8_En, BLACK, bg_color, 'a');

  // 根据当前设置类型绘制值
  if (index == SETTING_SHAKE_MODE)
  { // 振动模式
    // sprintf(strBuf, "%s", (*settings[index].value == ON) ? "on" : "off");

       if (*settings[index].value == ON) {
            strcpy(strBuf, "on");
        } else {
            strcpy(strBuf, "off");
        }
  }
  else
  {
    //   sprintf(strBuf, "%03d", *settings[index].value);
    intToStr(*settings[index].value, strBuf, 3);
  }

  Paint_DrawString(0 + Font8_En.Width * strlen(settings[index].name), index * 10, strBuf, &Font16_Num, BLACK, bg_color, '0');
}

void display_settings()
{
  for (int i = 0; i < SETTING_COUNT; i++)
  {
    draw_setting(i, i == current_setting);
  }
}

void handle_encoder_event()
{
  if (encode.state == ENCODE_EVENT_UP)
  { // 编码器向上滚动
    update_current_setting(*settings[current_setting].value + 1);
  }
  else if (encode.state == ENCODE_EVENT_DOWN)
  { // 编码器向下滚动
    update_current_setting(*settings[current_setting].value - 1);
  }
}

void handle_key_event()
{
  if (key.event == KEY_EVENT_CLICK)
  { // 确认键点击
    current_setting = (current_setting + 1) % SETTING_COUNT;
  }
}

void setting_page()
{
  display_settings();
  handle_encoder_event();
  handle_key_event();
}
void show_history_data()
{

  Paint_DrawString(2, 22, lora_receive_buf, &Font8_En, BLACK, WHITE, 'a');
}

void show_info(int posx, int posy, const char *label, int value, int offset)
{
  char strBuf[4];                                                    // 存储最多3位数字和一个终止符
  Paint_DrawString(posx, posy, label, &Font8_En, BLACK, WHITE, 'a'); // 绘制标签
                                                                     // sprintf(strBuf, "%03d:", value); // 显示3位数字
  intToStr(value, strBuf, 3);
  Paint_DrawString(posx + Font8_En.Width * offset, posy, strBuf, &Font16_Num, BLACK, WHITE, '0'); // 绘制值
}

void info_page()
{
  // 显示各项信息
  u16 sum = 0;

  // 计算电池平均值
  for (u8 i = 0; i < 10; i++)
  {
    sum += BattaryBuf[i];
  }
  sum /= 10;                                // 求平均值
  show_info(0, 0, "battery adc:", sum, 13); // 调用显示电池信息

  // 显示 Lora RSSI
  show_info(0, 10, "lora rssi:", SX1278_ReadRSSI(), 11); // 调用显示 Lora RSSI

  // 显示 Lora ID
  show_info(0, 30, "lora id:", SX1278_Read_Reg(REG_LR_VERSION), 9); // 调用显示 Lora ID

  // 显示屏幕亮度
  show_info(0, 40, "screen light:", TIM1->CH3CVR, 14); // 调用显示屏幕亮度
}
