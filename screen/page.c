/*
 * page.c
 *
 *  Created on: 2024年9月21日
 *      Author: 12630
 */
#include "page.h"
#include <string.h>
int8_t Englishcount = 0; // 字符的位号
int Englishposx = 0;     // x的个数
int Englishposy = 0;     // y的个数
u8 lora_send_buf[54];
// 定义设置数组
Setting settings[SETTING_COUNT] = {
    {"light", (u8 *)&TIM1->CH3CVR, NULL},
    {"shake", &shake_mode, NULL},
    {"freq", &Lora_Freq, SX1278_Init},
    {"power", &Lora_Power, SX1278_Init},
    {"lorabw", &Lora_BandWide, SX1278_Init},
    {"lorasf", &Lora_SpreadFactor, SX1278_Init}};

int current_setting = 0;

#define Font_WIDTH 7                                                                                      // 字体宽度
#define Font_HEIGH 12                                                                                     // 字体高度
#define EDGE 1                                                                                            // 边缘
#define OPERATE_DOWN 20                                                                                   // 操作界面下边缘
#define CHAT_DOWN 127                                                                                     // 输入框下边缘
#define COLUMN ((LCD_WIDTH - EDGE - EDGE) / Font_WIDTH)                                                   // 行个数18
#define CHAT_HISTORY_DOWN ((sizeof(lora_receive_buf) / COLUMN) * Font_HEIGH) + OPERATE_DOWN + EDGE + EDGE // 聊天记录下边缘5行90/18=5*12=60
#define CHAT_UP CHAT_HISTORY_DOWN + 2
void chat_page(sFONT *Font)
{

  show_battery();                                                                                    // 显示电池信息
  Paint_DrawChar(0, 0, 0, &Font16_Operate, BLACK, BLUE, 0);                                          // 绘制聊天页面的基本组件
  Paint_DrawRectangle(0, OPERATE_DOWN, 127, CHAT_HISTORY_DOWN, RED, DOT_PIXEL_1X1, DRAW_FILL_EMPTY); // 聊天记录界面
  Paint_DrawRectangle(0, CHAT_UP, 127, CHAT_DOWN, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);            // 输入框

  // 绘制字符
  Paint_DrawChar(EDGE + Englishposx * Font->Width, CHAT_UP + EDGE + Englishposy * Font->Height, 'a' + Englishcount, Font, BLACK, WHITE, 'a');

  Paint_DrawString(EDGE, CHAT_UP + EDGE, lora_send_buf, Font, BLACK, WHITE, 'a');

  show_history_data();
  // 处理滚动状态
  if (encode.state == ENCODE_EVENT_UP)
  {
    Englishcount = (Englishcount + 1) % 27; // 循环计数
  }
  else if (encode.state == ENCODE_EVENT_DOWN)
  {
    Englishcount = (Englishcount - 1 + 27) % 27; // 循环计数
  }

  if (lora_receive_flag == 2)
  {
    SX1278_LoRaTxPacket(lora_send_buf, Englishposx + Englishposy * (LCD_HEIGHT / Font->Width));
  }

  // 发送数据
  if (key.state == KEY_STATE_HOLD)
  {
    if (encode.state == ENCODE_EVENT_UP)
    {
      DEBUG_PRINT("start lora send\r\n");
      lora_receive_flag = 2;
      if (!SX1278_LoRaTxPacket(lora_send_buf, Englishposx + Englishposy * (LCD_HEIGHT / Font->Width)))
      {
        DEBUG_PRINT("lora send ok\r\n");
        SX1278_LoRaEntryRx(); // 进入接收模式
      }
      // 清空一切
      Englishposy = 0; // 重置行位置
      Englishposx = 0; // 重置行位置
      memset(lora_send_buf, 0, sizeof(lora_send_buf));

      Paint_DrawRectangle(EDGE, CHAT_UP + EDGE, 127 - EDGE, 127 - EDGE, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

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
          Englishposx = (LCD_WIDTH) / Font->Width - Font->Width; // 重置列位置
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
    lora_send_buf[Englishposx + Englishposy * (LCD_WIDTH / Font->Width)] = 'a' + Englishcount;
    Englishposx++;
    // DEBUG_PRINT("我111的数字=%d \r\n", EDGE + EDGE + Englishposx * Font->Width);

    if (EDGE + EDGE + (Englishposx + 1) * Font->Width > LCD_WIDTH)
    {
      Englishposx = 0;
      Englishposy++;
      //  DEBUG_PRINT("我的数字=%d \r\n", CHAT_UP + EDGE + Englishposy * Font->Height);
      if (CHAT_UP + EDGE + ((Englishposy + 1) * Font->Height) >= LCD_HEIGHT)
      {
        Englishposy = 0; // 重置行位置
      }
    }
    Englishcount = 0; // 重置计数
  }
}

void chat_history_page()
{

  // Paint_DrawRectangle(0, 20, 127, 100, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  // Paint_DrawRectangle(0, 102, 127, 127, BLUE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

  Paint_DrawRectangle(0, OPERATE_DOWN, 127, CHAT_HISTORY_DOWN, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY); // 聊天记录界面
  Paint_DrawRectangle(0, CHAT_UP, 127, CHAT_DOWN, BLUE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);               // 输入框

  show_history_data();
  if (encode.state == ENCODE_EVENT_UP) // 滚动
  {
  }

  if (encode.state == ENCODE_EVENT_DOWN) // 滚动
  {
  }
}
void perpare_setting_page()
{
  Paint_DrawRectangle(0, OPERATE_DOWN, 127, CHAT_HISTORY_DOWN, RED, DOT_PIXEL_1X1, DRAW_FILL_EMPTY); // 聊天记录界面
  Paint_DrawRectangle(0, CHAT_UP, 127, CHAT_DOWN, BLUE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);             // 输入框
  Paint_DrawChar(0, 0, 0, &Font16_Operate, GREEN, BLUE, 0);
}

void clamp_value(int *value, int min, int max)
{
  if (*value < min)
    *value = min;
  if (*value > max)
    *value = max;
}

void update_setting_value(int value, int min, int max, int current_setting)
{
  clamp_value(&value, min, max);
  *settings[current_setting].value = (u8)value;
}

void update_current_setting(int value)
{
  switch (current_setting)
  {
  case SETTING_SCREEN_LIGHT:
    value = (value < 0) ? 0 : (value > 100) ? 100
                                            : value;
    TIM1->CH3CVR = value;
    break;
  case SETTING_SHAKE_MODE:
    *settings[current_setting].value = !(*settings[current_setting].value);
    break;
  case SETTING_LORA_FREQ:
    update_setting_value(value, LORAFREQ_MIN, LORAFREQ_MAX, current_setting);
    break;
  case SETTING_LORA_POWER:
    update_setting_value(value, LORAPOWER_MIN, LORAPOWER_MAX, current_setting);
    break;
  case SETTING_LORA_BANDWIDTH:
    update_setting_value(value, LORABANDWIDTH_MIN, LORABANDWIDTH_MAX, current_setting);
    break;
  case SETTING_LORA_SPREAD_FACTOR:
    update_setting_value(value, LORASpreadFactor_MIN, LORASpreadFactor_MAX, current_setting);
    break;
  default:
    break;
  }

  if (settings[current_setting].update_func)
  {
    settings[current_setting].update_func();
  }
}

void draw_setting(int index, int highlight)
{
  char strBuf[4]; // 用于存储最多3位数字和一个终止符
  UWORD bg_color = highlight ? GREEN : WHITE;

  Paint_DrawString(0, index * 20, settings[index].name, &Font16_En, BLACK, bg_color, 'a');
  Paint_DrawChar(Font16_En.Width * strlen(settings[index].name), index * 20, 11, &Font16_Num, BLACK, bg_color, 0);

  // 根据当前设置类型绘制值
  if (index == SETTING_SHAKE_MODE)
  {
    if (*settings[index].value == ON)
    {
      strcpy(strBuf, "on");
    }
    else
    {
      strcpy(strBuf, "off");
    }
    Paint_DrawChar(Font16_En.Width * (strlen(settings[index].name) + 3), index * 20, 12, &Font16_Num, BLACK, BLACK, 0);
    Paint_DrawString(Font16_En.Width * (strlen(settings[index].name) + 1), index * 20, strBuf, &Font16_En, BLACK, bg_color, 'a');

    return;
  }
  else
  {
    intToStr(*settings[index].value, strBuf, 3);
  }

  Paint_DrawString(Font16_En.Width * (strlen(settings[index].name) + 1), index * 20, strBuf, &Font16_Num, BLACK, bg_color, '0');
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

  memset(lora_receive_buf + lora_receive_len, 0, sizeof(lora_receive_buf) - lora_receive_len);
  //   DEBUG_PRINT("lora_receive_len=%d\r\n", lora_receive_len);

  Paint_DrawString(1, 22, lora_receive_buf, &Font12_En, BLACK, WHITE, 'a');
}

void show_info(int posx, int posy, const char *label, int value, int offset)
{
  char strBuf[4];                                                     // 存储最多3位数字和一个终止符
  Paint_DrawString(posx, posy, label, &Font16_En, BLACK, WHITE, 'a'); // 绘制标签
  Paint_DrawChar(posx + Font16_En.Width * (offset - 1), posy, 11, &Font16_Num, BLACK, WHITE, 0);
  intToStr(value, strBuf, 3);
  Paint_DrawString(posx + Font16_Num.Width * offset, posy, strBuf, &Font16_Num, BLACK, WHITE, '0'); // 绘制值
}

void info_page()
{

  show_info(0, 0, "batadc", Battery_ADC_Average, 7);              // 调用显示电池信息
  show_info(0, 20, "rssi", SX1278_LoRaReadRSSI(), 5);             // 调用显示 Lora RSSI
  show_info(0, 40, "loraid", SX1278_Read_Reg(REG_LR_VERSION), 7); // 调用显示 Lora ID
  show_info(0, 60, "light", TIM1->CH3CVR, 6);                     // 调用显示屏幕亮度
}
