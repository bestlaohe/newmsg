/*
 * page.c
 *
 *  Created on: 2024��9��21��
 *      Author: 12630
 */
#include "page.h"
int8_t Englishcount = 0; // �ַ���λ��
int Englishposx = 0;     // �ַ���λ��
int Englishposy = 0;     // �ַ���λ��
static u8 lorasendbuf[100];

void chat_page(sFONT *Font)
{

  show_battery(); // ��ص�����ʾ����1412-264=1100
  Paint_DrawChar(0, 0, 0, &Font16_Operate, BLACK, BLUE, 0);
  Paint_DrawRectangle(0, 20, 127, 100, RED, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawRectangle(0, 102, 127, 127, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawChar(2 + Englishposx * Font->Width, 103 + Englishposy * Font->Height, 'a' + Englishcount, Font, BLACK, WHITE, 'a');
  //  show_history_data();

  if (encode.state == ENCODE_EVENT_UP) // ����
  {
    Englishcount++;
    if (Englishcount > 25)
      Englishcount = 0;
  }

  if (encode.state == ENCODE_EVENT_DOWN) // ����
  {
    Englishcount--;
    if (Englishcount < 0)
      Englishcount = 25;
  }

  // printf(" encode.state:%d, key.state:%d\r\n", encode.state, key.state);
  if (encode.state == ENCODE_EVENT_UP && key.state == KEY_STATE_HOLD) // ����
  {

    // int16_t i;
    // for (i = 0; i < Englishposx + Englishposy * (LCD_HEIGHT - 3 / Font->Width); i++)
    // {
    //   printf(" lorasendbuf[i]:%c\r\n", lorasendbuf[i]);
    // }

    if (!SX1278_LoRaTxPacket(lorasendbuf, Englishposx + Englishposy * (LCD_HEIGHT - 3 / Font->Width)))
    {
      printf("lora send ok \r\n");
    }

    key.enable = 0;
  }
  if (encode.state == ENCODE_EVENT_DOWN && key.state == KEY_STATE_HOLD) // �����ȥ
  {
    Englishposx--;

    if (Englishposx <= 0)
    {
      if (Englishposy > 0)
      {
        Englishposy--;

        Englishposx = LCD_WIDTH - 2 - (LCD_WIDTH - 2) / Font->Width;
      }
      else
      {
        Englishposx = 0;
      }
    }
    key.enable = 0;
  }

  //    printf(" Englishcount:%d\r\n", Englishcount);
  //    printf(" Font->Width:%d, Font->Height:%d\r\n", Font->Width, Font->Height);
  // printf(" 2 + Englishposx * Font->Width:%d\r\n", 2 + Englishposx * Font->Width);
  //
  // printf("Paint_DrawChar rg,Width = %d, Height = %d\r\n", Font->Width, Font->Height);

  if (key.event == KEY_EVENT_CLICK) // ȷ��
  {
    lorasendbuf[Englishposx + Englishposy * (LCD_HEIGHT - 3 / Font->Width)] = 'a' + Englishcount;
    Englishposx++;

    if (2 + Englishposx * Font->Width >= LCD_WIDTH - 1)
    {
      Englishposx = 0;
      Englishposy++;
      if (103 + Englishposy * Font->Height >= LCD_HEIGHT - 1)
      {
        Englishposy = 0;
      }
    }
    Englishcount = 0;
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

uint32_t *screen_light = TIM1->CH3CVR; // ��ʼ��Ļ����ֵ
extern int8_t shake_mode;
extern u8 Lora_Freq;         //  Ĭ��Ƶ������= LORAFREQ_434MHZ
extern u8 Lora_Power;        //  �����������= LORAPOWER_20DBM
extern u8 Lora_BandWide;     //  ��������= LORABW_125KHz
extern u8 Lora_SpreadFactor; //  ������Ƶ������7~12= 7

int current_setting = 0;    // ��ǰ����������
const int num_settings = 6; // ����������

void update_current_setting(int value)
{
  switch (current_setting)
  {
  case 0:
    *screen_light = value;
    break;
  case 1:
    shake_mode = value;
    break;
  case 2:
    Lora_Freq = value;
    SX1278_Init();
    break;
  case 3:
    Lora_Power = value;
    SX1278_Init();
    break;
  case 4:
    Lora_BandWide = value;
    SX1278_Init();
    break;
  case 5:
    Lora_SpreadFactor = value;
     SX1278_Init();
    break;
  }
}

void setting_page()
{
  char strBuf[5]; // Ҫ�洢���3λ���ֺ�һ����ֹ�������������СΪ4

  // ��Ļ��������
  Paint_DrawString(0, 0, "screen light:", &Font8_En, BLACK, WHITE, 'a');
  sprintf(strBuf, "%03d", screen_light);
  Paint_DrawString(0 + Font8_En.Width * 14, 0, strBuf, &Font16_Num, BLACK, WHITE, '0');

  // ��ģʽ����
  Paint_DrawString(0, 10, "shake mode:", &Font8_En, BLACK, WHITE, 'a');
  sprintf(strBuf, "%03d", shake_mode);
  Paint_DrawString(0 + Font8_En.Width * 12, 10, strBuf, &Font16_Num, BLACK, WHITE, '0');

  // LoRaƵ������
  Paint_DrawString(0, 30, "lora fre:", &Font8_En, BLACK, WHITE, 'a');
  sprintf(strBuf, "%03d", Lora_Freq);
  Paint_DrawString(0 + Font8_En.Width * 10, 30, strBuf, &Font16_Num, BLACK, WHITE, '0');

  // LoRa��������
  Paint_DrawString(0, 40, "lora power:", &Font8_En, BLACK, WHITE, 'a');
  sprintf(strBuf, "%03d", Lora_Power);
  Paint_DrawString(0 + Font8_En.Width * 12, 40, strBuf, &Font16_Num, BLACK, WHITE, '0');

  // LoRa��������
  Paint_DrawString(0, 50, "lora bandwidth:", &Font8_En, BLACK, WHITE, 'a');
  sprintf(strBuf, "%03d", Lora_BandWide);
  Paint_DrawString(0 + Font8_En.Width * 16, 50, strBuf, &Font16_Num, BLACK, WHITE, '0');

  // LoRa��Ƶ����
  Paint_DrawString(0, 60, "lora SpreadFactor:", &Font8_En, BLACK, WHITE, 'a');
  sprintf(strBuf, "%03d", Lora_SpreadFactor);
  Paint_DrawString(0 + Font8_En.Width * 19, 60, strBuf, &Font16_Num, BLACK, WHITE, '0');

  switch (current_setting)
  {
  case 0:
    Paint_DrawString(0, 0, "screen light:", &Font8_En, BLACK, GREEN, 'a');
    break;
  case 1:
    Paint_DrawString(0, 10, "shake mode:", &Font8_En, BLACK, GREEN, 'a');
    break;
  case 2:
    Paint_DrawString(0, 30, "lora fre:", &Font8_En, BLACK, GREEN, 'a');
    break;
  case 3:
    Paint_DrawString(0, 40, "lora power:", &Font8_En, BLACK, GREEN, 'a');
    break;
  case 4:
    Paint_DrawString(0, 50, "lora bandwidth:", &Font8_En, BLACK, GREEN, 'a');
    break;
  case 5:
    Paint_DrawString(0, 60, "lora SpreadFactor:", &Font8_En, BLACK, GREEN, 'a');
    break;
  }

  if (encode.state == ENCODE_EVENT_UP) // ���������Ϲ���
  {
    int value = 0;
    switch (current_setting)
    {
    case 0:
      value = screen_light + 1;
      break;
    case 1:
      value = shake_mode + 1;
      break;
    case 2:
      value = Lora_Freq + 1;
      break;
    case 3:
      value = Lora_Power + 1;
      break;
    case 4:
      value = Lora_BandWide + 1;
      break;
    case 5:
      value = Lora_SpreadFactor + 1;
      break;
    }
    update_current_setting(value);
  }

  if (encode.state == ENCODE_EVENT_DOWN) // ���������¹���
  {
    int value = 0;
    switch (current_setting)
    {
    case 0:
      value = screen_light - 1;
      break;
    case 1:
      value = shake_mode - 1;
      break;
    case 2:
      value = Lora_Freq - 1;
      break;
    case 3:
      value = Lora_Power - 1;
      break;
    case 4:
      value = Lora_BandWide - 1;
      break;
    case 5:
      value = Lora_SpreadFactor - 1;
      break;
    }
    update_current_setting(value);
  }

  if (key.event == KEY_EVENT_CLICK) // ȷ�ϼ����
  {

    current_setting = (current_setting + 1) % num_settings;
  }
}

void show_history_data()
{

  Paint_DrawString(2, 22, lorareceivebuf, &Font8_En, BLACK, WHITE, 'a');
}

void show_battery_info(int posx, int posy)
{

  u16 sum = 0;

  char strBuf[5]; // Ҫ�洢���3λ���ֺ�һ����ֹ�������������СΪ4
  for (u8 i = 0; i < 10; i++)
  {
    sum += BattaryBuf[i];
  }

  sum = sum / 10;

  Paint_DrawString(posx, posy, "battery adc:", &Font8_En, BLACK, WHITE, 'a'); // 13692

  sprintf(strBuf, "%04d:", sum);                                                              // ��ʾ2λ����
  Paint_DrawString(posx + Font8_En.Width * 13, posy, strBuf, &Font16_Num, BLACK, WHITE, '0'); // 13692
}
void show_lora_rssi(int posx, int posy)
{

  char strBuf[5]; // Ҫ�洢���3λ���ֺ�һ����ֹ�������������СΪ4

  //  printf("lora ID  0x%X\r\n", SX1278_Read_Reg(REG_LR_VERSION));
  //  printf("SX1278_LoRaReadRSSI= %d \r\n", SX1278_LoRaReadRSSI());
  //  printf("SX1278_ReadRSSI= %d \r\n", SX1278_ReadRSSI());

  Paint_DrawString(posx, posy, "lora rssi:", &Font8_En, BLACK, WHITE, 'a'); // 13692

  sprintf(strBuf, "%04d:", SX1278_ReadRSSI());                                                // ��ʾ2λ����
  Paint_DrawString(posx + Font8_En.Width * 11, posy, strBuf, &Font16_Num, BLACK, WHITE, '0'); // 13692
}
void show_lora_id(int posx, int posy)
{

  char strBuf[5]; // Ҫ�洢���3λ���ֺ�һ����ֹ�������������СΪ4

  Paint_DrawString(posx, posy, "lora id:", &Font8_En, BLACK, WHITE, 'a'); // 13692

  sprintf(strBuf, "%04d:", SX1278_Read_Reg(REG_LR_VERSION));                                 // ��ʾ2λ����
  Paint_DrawString(posx + Font8_En.Width * 9, posy, strBuf, &Font16_Num, BLACK, WHITE, '0'); // 13692
}
void show_screen_light(int posx, int posy)
{

  char strBuf[5]; // Ҫ�洢���3λ���ֺ�һ����ֹ�������������СΪ4

  Paint_DrawString(posx, posy, "lora id:", &Font8_En, BLACK, WHITE, 'a'); // 13692

  sprintf(strBuf, "%04d:", TIM1->CH3CVR);                                                    // ��ʾ2λ����
  Paint_DrawString(posx + Font8_En.Width * 9, posy, strBuf, &Font16_Num, BLACK, WHITE, '0'); // 13692
}
void info_page()
{

  show_battery_info(0, 0);

  show_lora_rssi(0, 10);
  show_lora_id(0, 30);

  show_screen_light(0, 40);
}
