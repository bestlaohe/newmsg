#include "debug.h"
#include "encode.h" // timeapp->encode->main
#include "adc.h"    // timeapp->pwm.screen->screen_api->adc->main
#include "SX1278.h" // gpio->SX1278->page
#include "page.h"


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


#define PWM_FRE         10000
#define PWM_Duty        50



// ���尴���¼�
typedef enum
{
  PAGE_SEND, // ����ҳ��
  PAGE_HISTROY_CHAT,
  PAGE_PERPARE_SETTING, // ׼������ҳ��
  PAGE_SETTING, // ����ҳ��
  PAGE_INFO,    // ��Ϣҳ��

} Page;

Page page = PAGE_SEND;

int main(void)
{

  /*********************�������ݳ�ʼ��******************************/
  SystemCoreClockUpdate();   // 48000000ϵͳʱ��ˢ��3324-3212=100k
  USART_Printf_Init(115200); // ���ڳ�ʼ����Ҫ�ڴ�ӡǰ����Ȼ�Ῠ��3956-3324=600k
  DEBUG_PRINT("\r\n\r\n\r\n\r\nSystemClk:%d\r\n", SystemCoreClock);
  //  DEBUG_PRINT("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
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
  Battery_Init();                                                    // ��ص�adc��ʼ��****9456-8636=820
  SX1278_Init();                                                     // lora�ĳ�ʼ��*****10268-9620=648
  EXTI_INT_INIT();                                                   // ��������磬lora�жϳ�ʼ��
  startup_animation();                                               // 11732-10512=500
  IWDG_Feed_Init(IWDG_Prescaler_256, 4000);                          // �ò��������ǽ��� 0 �� 0x0FFF ֮���һ����ֵ    // 4�벻ι���͸�λ   ��Ƶʱ���ڲ�128khz����256=500��1����500����4000=8s****12467-12356=111�ֽ�

  while (1)
  {
  // page = PAGE_SEND;
    switch (page) // ����ҳ��
    {
    case PAGE_SEND: // ���ͽ���
      chat_page(&Font12_En);
      if (key.event == KEY_EVENT_LONG_CLICK) // ����
      {
        page = PAGE_HISTROY_CHAT;
      }
      break;

    case PAGE_HISTROY_CHAT: // �����¼����
      chat_history_page();
      if (key.event == KEY_EVENT_LONG_CLICK)
        page = PAGE_PERPARE_SETTING;
      break;

    case PAGE_PERPARE_SETTING: // ׼�����ý���
      perpare_setting_page();
      if (key.event == KEY_EVENT_CLICK)
      {
        LCD_0IN85_Clear(BLACK);
        Paint_DrawChar(40, 40, 0, &Font24_icon, BLACK, BLUE, 0);
        Delay_Ms(500);
        LCD_0IN85_Clear(BLACK);
        page = PAGE_SETTING;
      }

      if (key.event == KEY_EVENT_LONG_CLICK)
      {
        LCD_0IN85_Clear(BLACK);
        page = PAGE_SEND;
      }
      break;

    case PAGE_SETTING: // ���ý���
      setting_page();
      if (key.event == KEY_EVENT_LONG_CLICK)
      {
        LCD_0IN85_Clear(BLACK);
        page = PAGE_INFO;
      }
      break;

    case PAGE_INFO: // ��Ϣ����
      info_page();
      if (key.event == KEY_EVENT_LONG_CLICK)
      {
        LCD_0IN85_Clear(BLACK);
        page = PAGE_SEND;
      }
      break;

    default: // Ĭ�����

      page = PAGE_SEND;
      break;
    }

    // �������¼�������¼�
    key.event = KEY_EVENT_NONE;
    encode.state = ENCODE_EVENT_NONE;
    SX1278_Receive();
    Encoder_Scan();
    IWDG_ReloadCounter();
    // Delay_Ms(200);
  }
}
