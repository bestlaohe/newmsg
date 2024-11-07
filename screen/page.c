/*
 * page.c
 *
 *  Created on: 2024��9��21��
 *      Author: 12630
 */
#include "page.h"
#include <string.h>
int8_t Englishcount = 0; // �ַ���λ��
int Englishposx = 0;     // x�ĸ���
int Englishposy = 0;     // y�ĸ���
u8 lora_send_buf[54];

Setting settings[SETTING_COUNT] = {
    {"light", (u8 *)&TIM1->CH3CVR, NULL},
    {"shake", &shake_mode, NULL},
    {"freq", &Lora_Freq, SX1278_Init},
    {"power", &Lora_Power, SX1278_Init},
    {"lorabw", &Lora_BandWide, SX1278_Init},
    {"lorasf", &Lora_SpreadFactor, SX1278_Init}};

int current_setting = 0;

Page page = PAGE_SEND;
int refreshState = 1;
#define EDGE 1                                                                                            // ��Ե
#define OPERATE_DOWN 20                                                                                   // ���������±�Ե
#define CHAT_DOWN 127                                                                                     // ������±�Ե
#define COLUMN ((LCD_WIDTH - EDGE - EDGE) / Font_WIDTH)                                                   // �и���18
#define CHAT_HISTORY_DOWN ((sizeof(lora_receive_buf) / COLUMN) * Font_HEIGH) + OPERATE_DOWN + EDGE + EDGE // �����¼�±�Ե5��90/18=5*12=60
#define CHAT_UP CHAT_HISTORY_DOWN + 2

void handle_chat_event(sFONT *Font)
{
  // �������״̬
  if (encode.state == ENCODE_EVENT_UP)
  {
    refreshState = 1;
    Englishcount = (Englishcount + 1) % 27; // ѭ������
  }
  else if (encode.state == ENCODE_EVENT_DOWN)
  {
    refreshState = 1;
    Englishcount = (Englishcount - 1 + 27) % 27; // ѭ������
  }

  if (lora_receive_flag == 2)
  {
    refreshState = 1;
    SX1278_LoRaTxPacket(lora_send_buf, Englishposx + Englishposy * (LCD_HEIGHT / Font->Width));
  }

  // ��������
  if (key.state == KEY_STATE_HOLD)
  {
    if (encode.state == ENCODE_EVENT_UP)
    {
      refreshState = 1;
      DEBUG_PRINT("start lora send\r\n");
      lora_receive_flag = 2;
      if (!SX1278_LoRaTxPacket(lora_send_buf, Englishposx + Englishposy * (LCD_HEIGHT / Font->Width)))
      {
        DEBUG_PRINT("lora send ok\r\n");
        SX1278_LoRaEntryRx(); // �������ģʽ
      }
      // ���һ��
      Englishposy = 0; // ������λ��
      Englishposx = 0; // ������λ��
      memset(lora_send_buf, 0, sizeof(lora_send_buf));

      LCD_0IN85_Clear(EDGE, CHAT_UP + EDGE, 127 - EDGE, 127 - EDGE, MY_THEME_BACK_COLOR); // �����������

      key.enable = 0; // ���ü�
    }
    else if (encode.state == ENCODE_EVENT_DOWN)
    {
      refreshState = 1;
      // �������
      if (--Englishposx < 0)
      {
        if (Englishposy > 0)
        {
          Englishposy--;
          Englishposx = (LCD_WIDTH) / Font->Width - Font->Width; // ������λ��
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
    refreshState = 1;
    lora_send_buf[Englishposx + Englishposy * (LCD_WIDTH / Font->Width)] = 'a' + Englishcount;
    Englishposx++;
    // DEBUG_PRINT("��111������=%d \r\n", EDGE + EDGE + Englishposx * Font->Width);

    if (EDGE + EDGE + (Englishposx + 1) * Font->Width > LCD_WIDTH)
    {
      Englishposx = 0;
      Englishposy++;
      //  DEBUG_PRINT("�ҵ�����=%d \r\n", CHAT_UP + EDGE + Englishposy * Font->Height);
      if (CHAT_UP + EDGE + ((Englishposy + 1) * Font->Height) >= LCD_HEIGHT)
      {
        Englishposy = 0; // ������λ��
      }
    }
    Englishcount = 0; // ���ü���
  }
}

int current_line = 0; // ��ǰ��ʾ����ʼ��
int total_lines = 0;  // �ܵ������¼����

void handle_chat_history_event()
{
  if (encode.state == ENCODE_EVENT_UP) // ���Ϲ���
  {
    if (current_line > 0)
    {
      current_line--;
      refreshState = 1;
    }
  }

  if (encode.state == ENCODE_EVENT_DOWN) // ���¹���
  {
    if (current_line < total_lines - 1)
    {
      current_line++;
      refreshState = 1;
    }
  }

  if (key.state == KEY_STATE_HOLD) // ɾ�������¼
  {
    if (encode.state == ENCODE_EVENT_DOWN)
    {
      refreshState = 1;
      memset(lora_receive_buf, 0, sizeof(lora_receive_len)); // ��������¼
      total_lines = 0;
      current_line = 0;
      key.enable = 0; // ���ü�
    }
  }
}
void show_history_data(sFONT *Font)
{
  int max_lines_to_display = 5; // ������Ļһ������ʾ5��
  int start_line = current_line;
  int end_line = current_line + max_lines_to_display; // 5

  if (end_line > total_lines)
  {
    end_line = total_lines;
  }

  // �����ʾ����
  LCD_0IN85_Clear(EDGE, OPERATE_DOWN + EDGE, 127 - EDGE, CHAT_HISTORY_DOWN - EDGE, MY_SCREEN_COLOR); // �����������
  // ��ʾ��ǰ��Χ�ڵ������¼
  for (int i = start_line; i < end_line; i++)
  {
    Paint_DrawString(1, 22 + (i - start_line) * Font->Height, lora_receive_buf, Font, MY_THEME_BACK_COLOR, MY_THEME_COMPONT_COLOR, 'a');
  }
}

void chat_page(sFONT *Font)
{

  show_battery(BATTERY_X, BATTERY_Y, MY_THEME_BACK_COLOR, MY_THEME_COMPONT_COLOR); // ��ʾ�����Ϣ

  if (refreshState)
  {
    Paint_DrawChar(1, 1, 0, &Font16_Operate, MY_THEME_BACK_COLOR, MY_THEME_COMPONT_COLOR, 0); // ���õ�ͼ��
    LCD_0IN85_Clear(0, OPERATE_DOWN, 127, CHAT_HISTORY_DOWN, MY_SCREEN_COLOR);                // �����¼����
    LCD_0IN85_Clear(0, CHAT_UP, 127, CHAT_DOWN, MY_SCREEN_COLOR);                             // �����
    Paint_DrawRectangle(0, CHAT_UP, 127, CHAT_DOWN, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);   // ��������

    // �����ַ�
    Paint_DrawChar(EDGE + Englishposx * Font->Width, CHAT_UP + EDGE + Englishposy * Font->Height, 'a' + Englishcount, Font, MY_THEME_BACK_COLOR, MY_THEME_COMPONT_COLOR, 'a');
    Paint_DrawString(EDGE, CHAT_UP + EDGE, lora_send_buf, Font, MY_THEME_BACK_COLOR, MY_THEME_COMPONT_COLOR, 'a');

    show_history_data(Font);
    refreshState = 0;
  }

  handle_chat_event(Font);
}
void chat_history_page(sFONT *Font)
{
  show_battery(BATTERY_X, BATTERY_Y, MY_THEME_BACK_COLOR, MY_THEME_COMPONT_COLOR);
  if (refreshState)
  {
    Paint_DrawRectangle(0, OPERATE_DOWN, 127, CHAT_HISTORY_DOWN, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY); // �����¼�������
    LCD_0IN85_Clear(0, CHAT_UP, 128, CHAT_DOWN, MY_SCREEN_COLOR);                                        // �����

    show_history_data(Font);
    refreshState = 0;
  }

  handle_chat_history_event();
}

void perpare_setting_page(sFONT *Font)
{
  show_battery(BATTERY_X, BATTERY_Y, MY_THEME_BACK_COLOR, MY_THEME_COMPONT_COLOR); // ������
  if (refreshState)
  {
    LCD_0IN85_Clear(0, OPERATE_DOWN, 128, CHAT_HISTORY_DOWN, MY_SCREEN_COLOR); // �����¼����
    Paint_DrawChar(0, 0, 0, &Font16_Operate, GREEN, BLUE, 0);                  // ����ҳ�����
    show_history_data(Font);
    refreshState = 0;
  }
}

/**************************************����ҳ��**************************************************** */
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

#define CHARHEIGHT 18
#define Y_OFFSET 22
void draw_setting(int index, int highlight, sFONT *Font)
{

  char strBuf[4]; // ���ڴ洢���3λ���ֺ�һ����ֹ��
  UWORD bg_color = highlight ? GREEN : MY_THEME_COMPONT_COLOR;

  Paint_DrawString(0, index * CHARHEIGHT + Y_OFFSET, settings[index].name, Font, MY_THEME_BACK_COLOR, bg_color, 'a');
  Paint_DrawChar(Font->Width * strlen(settings[index].name), index * CHARHEIGHT + Y_OFFSET, 11, &Font16_Num, MY_THEME_BACK_COLOR, bg_color, 0);

  // ���ݵ�ǰ�������ͻ���ֵ
  if (index == SETTING_SHAKE_MODE)
  { // �հ�
    Paint_DrawChar(Font->Width * (strlen(settings[index].name) + 3), index * CHARHEIGHT + Y_OFFSET, 12, &Font16_Num, MY_THEME_BACK_COLOR, MY_THEME_BACK_COLOR, 0);

    if (*settings[index].value == ON)
    {
      Paint_DrawString(Font->Width * (strlen(settings[index].name) + 1), index * CHARHEIGHT + Y_OFFSET, "0", &Font16_button, MY_THEME_BACK_COLOR, GREEN, '0');
      Paint_DrawString(Font->Width * (strlen(settings[index].name) + 1) + 15, index * CHARHEIGHT + Y_OFFSET + 2, "0", &Font16_cycle, GREEN, WHITE, '0');
    }
    else
    {
      Paint_DrawString(Font->Width * (strlen(settings[index].name) + 1), index * CHARHEIGHT + Y_OFFSET, "0", &Font16_button, MY_THEME_BACK_COLOR, RED, '0');
      Paint_DrawString((Font->Width * (strlen(settings[index].name) + 1)) + 2, index * CHARHEIGHT + Y_OFFSET + 2, "0", &Font16_cycle, RED, WHITE, '0');
    }

    return;
  }
  else
  {
    intToStr(*settings[index].value, strBuf, 3);
  }

  Paint_DrawString(Font->Width * (strlen(settings[index].name) + 1), index * CHARHEIGHT + Y_OFFSET, strBuf, &Font16_Num, MY_THEME_BACK_COLOR, bg_color, '0');
}

void display_settings(sFONT *Font)
{

  if (refreshState)
  {
    for (int i = 0; i < SETTING_COUNT; i++)
    {
      draw_setting(i, i == current_setting, Font);
    }
    refreshState = 0;
  }
}

void handle_setting_event()
{
  if (encode.state == ENCODE_EVENT_UP)
  { // ���������Ϲ���
    update_current_setting(*settings[current_setting].value + 1);
    refreshState = 1;
  }
  else if (encode.state == ENCODE_EVENT_DOWN)
  { // ���������¹���
    update_current_setting(*settings[current_setting].value - 1);
    refreshState = 1;
  }
  if (key.event == KEY_EVENT_CLICK)
  { // ȷ�ϼ����
    current_setting = (current_setting + 1) % SETTING_COUNT;
    refreshState = 1;
  }
}

void setting_page(sFONT *Font)
{

  Paint_DrawString(48, 0, "01", &Font16_setting, MY_THEME_BACK_COLOR, MY_THEME_COMPONT_COLOR, '0');

  display_settings(Font);
  handle_setting_event();
}

/**************************************����ҳ��**************************************************** */

void show_info(int posx, int posy, const char *label, int value, int offset)
{
  char strBuf[4];                                                                                    // �洢���3λ���ֺ�һ����ֹ��
  Paint_DrawString(posx, posy, label, &Font16_En, MY_THEME_BACK_COLOR, MY_THEME_COMPONT_COLOR, 'a'); // ���Ʊ�ǩ
  Paint_DrawChar(posx + Font16_En.Width * (offset - 1), posy, 11, &Font16_Num, MY_THEME_BACK_COLOR, MY_THEME_COMPONT_COLOR, 0);
  intToStr(value, strBuf, 3);
  Paint_DrawString(posx + Font16_Num.Width * offset, posy, strBuf, &Font16_Num, MY_THEME_BACK_COLOR, MY_THEME_COMPONT_COLOR, '0'); // ����ֵ
}

void info_page()
{

  show_info(0, 0, "batadc", Battery_ADC_Average, 7);              // ������ʾ�����Ϣ
  show_info(0, 20, "rssi", SX1278_LoRaReadRSSI(), 5);             // ������ʾ Lora RSSI
  show_info(0, 40, "loraid", SX1278_Read_Reg(REG_LR_VERSION), 7); // ������ʾ Lora ID
  show_info(0, 60, "light", TIM1->CH3CVR, 6);                     // ������ʾ��Ļ����
}

void show_page()
{

  // page = PAGE_SETTING;
  switch (page) // ����ҳ��
  {
  case PAGE_SEND: // ���ͽ���
    chat_page(&Font12_En);
    if (key.event == KEY_EVENT_LONG_CLICK) // ����
    {
      refreshState = 1;
      DEBUG_PRINT("PAGE_HISTROY_CHAT\r\n");
      page = PAGE_HISTROY_CHAT;
    }
    break;

  case PAGE_HISTROY_CHAT: // �����¼����  ��Ҫ�����鿴��¼
    chat_history_page(&Font12_En);
    if (key.event == KEY_EVENT_LONG_CLICK)
    {
      refreshState = 1;
      page = PAGE_PERPARE_SETTING;
    }

    break;

  case PAGE_PERPARE_SETTING: // ׼�����ý���
    perpare_setting_page(&Font12_En);
    if (key.event == KEY_EVENT_CLICK)
    {
      refreshState = 1;
      LCD_0IN85_Clear(0, 0, 127, 127, MY_THEME_BACK_COLOR);

      // // ����һ���������õĵط�
      // Paint_DrawChar(40, 40, 0, &Font24_icon, MY_THEME_BACK_COLOR, BLUE, 0);
      // Delay_Ms(500);
      // // ����ɾ��

      // LCD_0IN85_Clear(0, 0, 127, 127, MY_THEME_BACK_COLOR);
      page = PAGE_SETTING;
    }

    if (key.event == KEY_EVENT_LONG_CLICK)
    {
      refreshState = 1;
      LCD_0IN85_Clear(0, 0, 127, 127, MY_THEME_BACK_COLOR);
      page = PAGE_SEND;
    }
    break;

  case PAGE_SETTING: // ���ý���
    setting_page(&Font16_En);
    if (key.event == KEY_EVENT_LONG_CLICK)
    {
      refreshState = 1;
      LCD_0IN85_Clear(0, 0, 127, 127, MY_THEME_BACK_COLOR);
      // page = PAGE_INFO;
      page = PAGE_SEND;
    }
    break;

    // case PAGE_INFO: // ��Ϣ����
    //   info_page();
    //   if (key.event == KEY_EVENT_LONG_CLICK)
    //   {
    //     LCD_0IN85_Clear(0,0,128,128,MY_THEME_BACK_COLOR);
    //     page = PAGE_SEND;
    //   }
    //   break;

  default: // Ĭ�����
    page = PAGE_SEND;
    break;
  }

  // �������¼�������¼�
  key.event = KEY_EVENT_NONE;
  encode.state = ENCODE_EVENT_NONE;
}

// ��д���ֽṹ-
// ÿ��page��ҳ��������ʾ
// ÿ��ҳ��Ĳ���
// show_page��ʾ�ܵ�page����

// ������ʾ
// �߼�����
