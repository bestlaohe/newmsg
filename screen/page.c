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
u8 lorasendbuf[255];
u8 lorareceivebuf[255];

void chat_page()
{

  show_battery(); // ��ص�����ʾ����1412-264=1100
  Paint_Drawicon(0, 0, 0, &Font16_Operate, BLACK, BLUE);
  Paint_DrawRectangle(0, 20, 127, 100, RED, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawRectangle(0, 102, 127, 127, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

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

    int16_t i;
    for (i = 0; i < Englishposx + Englishposy * (LCD_HEIGHT - 3 / Font8_En.Width); i++)
    {
      printf(" lorasendbuf[i]:%c\r\n", lorasendbuf[i]);
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

        Englishposx = LCD_WIDTH - 2 - (LCD_WIDTH - 2) / Font8_En.Width;
      }
      else
      {
        Englishposx = 0;
      }
    }
    key.enable = 0;
  }

  //  printf(" Englishcount:%d\r\n", Englishcount);

  Paint_DrawChar(2 + Englishposx * Font8_En.Width, 103 + Englishposy * Font8_En.Height, 'a' + Englishcount, &Font8_En, BLACK, WHITE, 'a');

  if (key.event == KEY_EVENT_CLICK) // ȷ��
  {
    lorasendbuf[Englishposx + Englishposy * (LCD_HEIGHT - 3 / Font8_En.Width)] = 'a' + Englishcount;
    Englishposx++;
    printf(" 2+Englishposx * Font8_En.Width:%d\r\n", 2 + Englishposx * Font8_En.Width);
    if (2 + Englishposx * Font8_En.Width >= LCD_WIDTH - 1)
    {

      Englishposx = 0;
      Englishposy++;
      if (103 + Englishposy * Font8_En.Height >= LCD_HEIGHT - 1)
      {
        Englishposy = 0;
      }
    }
    Englishcount = 0;
  }
}

void chat_history_page()
{

  if (encode.state == ENCODE_EVENT_UP) // ����
  {
  }

  if (encode.state == ENCODE_EVENT_DOWN) // ����
  {
  }
}
