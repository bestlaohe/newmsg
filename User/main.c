
#include "debug.h"
#include "encode.h"
#include "adc.h"
#include "SX1278.h"
#include "page.h"

u16 BattaryBuf[10];

//******************guipain
//**********************|
//**********************|
// timeapp.pwm.screen.screen_api.adc.main
// timeapp.encode.main
// gpio.SX1278.main

// ����������ͣ�˯�߹������
// 16k=16,384
// data�Ǳ�ʾ����Σ�text segment���Ĵ�С�����ֽ�Ϊ��λ������ΰ����˳���Ŀ�ִ��ָ�
// bss�Ǵ洢��Щ��Դ������û����ʽ��ʼ����ȫ�ֱ����;�̬������
// data����ʽ��ʼ����ȫ�ֱ����;�̬����

//  ROM��
//  text �Σ��洢������롣
//  data �Σ��洢�ѳ�ʼ����ȫ�ֱ����;�̬�����ĳ�ʼֵ��

//  RAM��
//  text �Σ�������ʱ���ڴ洢����ִ�У���ֱ��ռ�� RAM �Ĵ洢�ռ䣬����Ҫ RAM ��ִ�У���
//  data �Σ�������ʱ�洢�ѳ�ʼ����ȫ�ֱ����;�̬������
//  bss �Σ�������ʱ�洢δ��ʼ����ȫ�ֱ����;�̬������

#define PWM_FRE 10000
#define PWM_Duty 100

// ���尴���¼�
typedef enum
{
  PAGE_SEND, // ����ҳ��
  PAGE_HISTROY_CHAT,
  PAGE_PERPARE_SETTING, // ����ҳ��

  PAGE_SETTING, // ����ҳ��
  PAGE_INFO,    // ��Ϣҳ��
} Page;

Page page = PAGE_SEND;

extern Key key;

extern Encode encode;

int main(void)
{


  /*********************�������ݳ�ʼ��******************************/
  SystemCoreClockUpdate();   // 48000000ϵͳʱ��ˢ��3324-3212=100k
  USART_Printf_Init(115200); // ���ڳ�ʼ����Ҫ�ڴ�ӡǰ����Ȼ�Ῠ��3956-3324=600k
  printf("SystemClk:%d\r\n", SystemCoreClock);
  //  printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
  Delay_Init(); // ��ʱ��ʼ����Ҫ����ʱǰ����Ȼ�Ῠ��4012-3956=100�ֽ�
  FLASH_Unlock();
  FLASH_UserOptionByteConfig(OB_IWDG_SW, OB_STDBY_NoRST, OB_RST_NoEN, OB_PowerON_Start_Mode_BOOT);
  FLASH_Lock(); //**4232-4012=200�ֽ�
  /*********************Ӧ�ú�����ʼ��******************************/
  My_GPIO_Init();                                                    // IO�ڳ�ʼ��****4484-4232=252�ֽ�
  TIM1_Init(100, (SystemCoreClock / (100 * PWM_FRE)) - 1, PWM_Duty); // ��Ļ�ı������  Ĭ�ϰٷְ�����******5076-4484=592�ֽ�pwmҪ200��+��ʱ��300
  TIM2_Init(11, 1);                                                  // ������������,����ֵΪ65535������Ƶ��1Ȧ12��****6020-6900=880�ֽ����벶��Ҫ500��+��ʱ��300
  LCD_Drive_Init();                                                  // ��ĻӲ����ʼ��****200�ֽ�
  LCD_SHOW_API_INIT();                                               // ��Ļ����******8404-6224=2180
  KEY_INIT();                                                        // ȷ�ϰ����жϳ�ʼ��***8636-8404=232
  Battery_Init();                                                    // ��ص�adc��ʼ��****9456-8636=820
  EXTI7_INT_INIT();                                                  // �ⲿ���������жϣ���ʼ���*****9540=100
  EXTI6_INT_INIT();                                                  // �ⲿ���������жϣ�lora����Ϣ������*****9620=100
  SX1278_Init(434);                                                  // lora�ĳ�ʼ��*****10268-9620=648
  startup_animation();                                               // 11732-10512=1220
  IWDG_Feed_Init(IWDG_Prescaler_128, 10000);                         // 4�벻ι���͸�λ   ��Ƶʱ���ڲ�128khz����128=1000��1����1000����4000=4s****12467-12356=111�ֽ�

  while (1)
  {
    LCD_0IN85_SetBackLight(50);
     Paint_DrawLine(0, 4, 0, 20, BLUE, 1, LINE_STYLE_SOLID);
    //  Paint_DrawRectangle(0, 0, 50, 50, BLUE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    //  Paint_DrawRectangle(0, 100, 127, 127, RED, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

    switch (page) // ����ҳ��
    {
    case PAGE_SEND: // ���ͽ���

     // chat_page();

      if (key.event == KEY_EVENT_LONG_CLICK) // ����

        page = PAGE_HISTROY_CHAT;

      break;

    case PAGE_HISTROY_CHAT: // �����¼����

      Paint_DrawRectangle(0, 20, 127, 100, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      Paint_DrawRectangle(0, 102, 127, 127, BLUE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      chat_history_page();

      if (key.event == KEY_EVENT_LONG_CLICK)
        page = PAGE_PERPARE_SETTING;
      break;

    case PAGE_PERPARE_SETTING: // ���ý���

      Paint_DrawRectangle(0, 20, 127, 100, RED, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      Paint_DrawRectangle(0, 102, 127, 127, BLUE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      Paint_Drawicon(0, 0, 0, &Font16_Operate, GREEN, BLUE);
      if (key.event == KEY_EVENT_CLICK)
      {
        Paint_Clear(BLACK);
        page = PAGE_SETTING;
      }

      if (key.event == KEY_EVENT_LONG_CLICK)
        page = PAGE_SEND;
      break;

    case PAGE_SETTING: // ���ý���

      Paint_Drawicon(40, 40, 0, &Font24_icon, BLACK, BLUE);

      if (key.event == KEY_EVENT_LONG_CLICK)
      {
        Paint_Clear(BLACK);
        page = PAGE_INFO;
      }
      break;

    case PAGE_INFO: // ��Ϣ����

      if (key.event == KEY_EVENT_LONG_CLICK)
        page = PAGE_SEND;
      break;

    default: // Ĭ�����

      page = PAGE_SEND;
      break;
    }

    // �������¼�������¼�
    key.event = KEY_EVENT_NONE;
    encode.state = ENCODE_EVENT_NONE;

    //   Paint_DrawString(77, 0, "ac", &Font24_En, BLACK, WHITE, '0'); // 14272-11732=2540
    // Paint_DrawString(77, 0, "ac", &Font8_En, BLACK, WHITE, '0'); // 12608-11732=876

    // SX1278_test(); //  16180-15028=1652             �����368
    Encoder_Scan();
    IWDG_ReloadCounter(); // ι��* 12484-12467=24�ֽ�
    Delay_Ms(10);
  }
}
