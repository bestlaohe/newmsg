
#include "debug.h"
#include "encode.h"
#include "adc.h"
#include "SX1278.h"

extern volatile int circle;

u16 BattaryBuf[10];

//******************guipain
//**********************|
//**********************|
// timeapp->pwm->screen->screen_api->adc->main
// timeapp->encode->main
// gpio->SX1278->main

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
  PAGE_SEND,    // ����ҳ��
  PAGE_SETTING, // ����ҳ��
  PAGE_INFO,    // ��Ϣҳ��
} Page;

Page page = PAGE_SEND;

extern Key key;
int main(void)
{
  uint16_t precnt = 0;
  int precircle = 0;

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
  TIM2_Init(12, 1);                                                  // ������������,����ֵΪ65535������Ƶ��1Ȧ12��****6020-6900=880�ֽ����벶��Ҫ500��+��ʱ��300
  LCD_Drive_Init();                                                  // ��ĻӲ����ʼ��****200�ֽ�
  LCD_SHOW_API_INIT();                                               // ��Ļ����******8404-6224=2180
  KEY_INIT();                                                        // ȷ�ϰ����жϳ�ʼ��***8636-8404=232
  Battery_Init();                                                    // ��ص�adc��ʼ��****9456-8636=820
  EXTI7_INT_INIT();                                                  // �ⲿ���������жϣ���ʼ���*****9540=100
  EXTI6_INT_INIT();                                                  // �ⲿ���������жϣ�lora����Ϣ������*****9620=100
  SX1278_Init(434);                                                  // lora�ĳ�ʼ��*****10268-9620=648
                                                                     // startup_animation();                                               // 11732-10512=1220
  IWDG_Feed_Init(IWDG_Prescaler_128, 10000);                         // 4�벻ι���͸�λ   ��Ƶʱ���ڲ�128khz����128=1000��1����1000����4000=4s****12467-12356=111�ֽ�

  while (1)
  {

    // ����ҳ��
    switch (page)
    {
    case PAGE_SEND: // ���ͽ���

      show_battery(); // ��ص�����ʾ����16124-15028=1612
      Paint_DrawRectangle(0, 20, 128, 120,BLUE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      
      if (key->event = KEY_EVENT_HOLD)
        page = PAGE_SETTING;
      break;

    case PAGE_SETTING: // ���ý���

      if (key->event = KEY_EVENT_HOLD)
        page = PAGE_INFO;
      break;

    case PAGE_INFO: // ��Ϣ����

      if (key->event = KEY_EVENT_HOLD)
        page = PAGE_SEND;
      break;

    default: // Ĭ�����

      page = PAGE_SEND;
      break;
    }

    // �������¼�������¼�
    key->event = KEY_EVENT_NONE;

    IWDG_ReloadCounter(); // ι��* 12484-12467=24�ֽ�

    //   Paint_DrawString(77, 0, "ac", &Font24_En, BLACK, WHITE, '0'); // 14272-11732=2540
    // Paint_DrawString(77, 0, "ac", &Font8_En, BLACK, WHITE, '0'); // 12608-11732=876

    //  SX1278_test(); //  16180-15028=1652             �����368

    //    if (precircle != circle || (precnt != TIM2->CNT)) // �б仯�Ͷ�   140�ֽ�
    //    {
    //      printf("Encoder position= %d circle %d step\r\n", TIM2->CNT, circle);
    //      precircle = circle;
    //      precnt = TIM2->CNT;
    //      system_wokeup();
    //      MOTOR_ON;
    //      Delay_Ms(50);
    //      MOTOR_OFF;
    //    }
    Delay_Ms(100);
  }
}
