/*
 * page.c
 *
 *  Created on: 2024��9��21��
 *      Author: 12630
 */
#include "page.h"
#include <string.h>
int8_t Englishcount = 0; // �ַ���λ��
int Englishposx = 0;     // �ַ���λ��
int Englishposy = 0;     // �ַ���λ��
static u8 lora_send_buf[50];
// ������������
Setting settings[SETTING_COUNT] = {
    {"light", (u8 *)&TIM1->CH3CVR, NULL},
    {"shake", &shake_mode, NULL},
    {"freq", &Lora_Freq, SX1278_Init},
    {"power", &Lora_Power, SX1278_Init},
    {"lorabw", &Lora_BandWide, SX1278_Init},
    {"lorasf", &Lora_SpreadFactor, SX1278_Init}};

int current_setting = 0;
void chat_page(sFONT *Font)
{

  show_battery();                                           // ��ʾ�����Ϣ
  Paint_DrawChar(0, 0, 0, &Font16_Operate, BLACK, BLUE, 0); // ��������ҳ��Ļ������
  Paint_DrawRectangle(0, 20, 127, 100, RED, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawRectangle(0, 102, 127, 127, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

  // �����ַ�
  Paint_DrawChar(2 + Englishposx * Font->Width, 103 + Englishposy * Font->Height, 'a' + Englishcount, Font, BLACK, WHITE, 'a');

  Paint_DrawString(0, 103 + Englishposy * Font->Height, lora_send_buf, Font, BLACK, WHITE, 'a');

  DEBUG_PRINT("chat_page5\r\n");

  // �������״̬
  if (encode.state == ENCODE_EVENT_UP)
  {
    Englishcount = (Englishcount + 1) % 26; // ѭ������
  }
  else if (encode.state == ENCODE_EVENT_DOWN)
  {
    Englishcount = (Englishcount - 1 + 26) % 26; // ѭ������
  }

  // ��������
  if (key.state == KEY_STATE_HOLD)
  {
    if (encode.state == ENCODE_EVENT_UP)
    {
      // ��������
      if (!SX1278_LoRaTxPacket(lora_send_buf, Englishposx + Englishposy * (LCD_HEIGHT - 3 / Font->Width)))
      {
        DEBUG_PRINT("lora send ok \r\n");
      }
      key.enable = 0; // ���ü�
    }
    else if (encode.state == ENCODE_EVENT_DOWN)
    {
      // �������
      if (--Englishposx < 0)
      {
        if (Englishposy > 0)
        {
          Englishposy--;
          Englishposx = (LCD_WIDTH - 2) / Font->Width - 1; // ������λ��
        }
        else
        {
          Englishposx = 0;
        }
      }
      key.enable = 0; // ���ü�
    }
  }

  // ȷ������
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
        Englishposy = 0; // ������λ��
      }
    }
    Englishcount = 0; // ���ü���
  }
}

void chat_history_page()
{
  Paint_DrawRectangle(0, 20, 127, 100, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawRectangle(0, 102, 127, 127, BLUE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  show_history_data();
  if (encode.state == ENCODE_EVENT_UP) // ����
  {
  }

  if (encode.state == ENCODE_EVENT_DOWN) // ����
  {
  }
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
  char strBuf[4]; // ���ڴ洢���3λ���ֺ�һ����ֹ��
  UWORD bg_color = highlight ? GREEN : WHITE;

  Paint_DrawString(0, index * 20, settings[index].name, &Font16_En, BLACK, bg_color, 'a');
  Paint_DrawChar(Font16_En.Width * strlen(settings[index].name), index * 20, 11, &Font16_Num, BLACK, bg_color, 0);

  // ���ݵ�ǰ�������ͻ���ֵ
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
  { // ���������Ϲ���
    update_current_setting(*settings[current_setting].value + 1);
  }
  else if (encode.state == ENCODE_EVENT_DOWN)
  { // ���������¹���
    update_current_setting(*settings[current_setting].value - 1);
  }
}

void handle_key_event()
{
  if (key.event == KEY_EVENT_CLICK)
  { // ȷ�ϼ����
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
  char strBuf[4];                                                     // �洢���3λ���ֺ�һ����ֹ��
  Paint_DrawString(posx, posy, label, &Font16_En, BLACK, WHITE, 'a'); // ���Ʊ�ǩ
  Paint_DrawChar(posx + Font16_En.Width * (offset - 1), posy, 11, &Font16_Num, BLACK, WHITE, 0);
  intToStr(value, strBuf, 3);
  Paint_DrawString(posx + Font16_Num.Width * offset, posy, strBuf, &Font16_Num, BLACK, WHITE, '0'); // ����ֵ
}

void info_page()
{

  show_info(0, 0, "batadc", Battery_ADC_Average, 7);          // ������ʾ�����Ϣ
  show_info(0, 20, "rssi", SX1278_ReadRSSI(), 5);             // ������ʾ Lora RSSI
  show_info(0, 40, "id", SX1278_Read_Reg(REG_LR_VERSION), 3); // ������ʾ Lora ID
  show_info(0, 60, "light", TIM1->CH3CVR, 6);                 // ������ʾ��Ļ����
}
