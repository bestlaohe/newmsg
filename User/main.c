
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
int main(void)
{
  uint16_t precnt = 0;
  int precircle = 0;

  /*********************�������ݳ�ʼ��******************************/
  SystemCoreClockUpdate();   // 48000000ϵͳʱ��ˢ��3324-3212=100k
  USART_Printf_Init(115200); // ���ڳ�ʼ����Ҫ�ڴ�ӡǰ����Ȼ�Ῠ��3956-3324=600k
//  printf("SystemClk:%d\r\n", SystemCoreClock);
//  printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
  Delay_Init(); // ��ʱ��ʼ����Ҫ����ʱǰ����Ȼ�Ῠ��4012-3956=100�ֽ�

  FLASH_Unlock();
  FLASH_UserOptionByteConfig(OB_IWDG_SW, OB_STDBY_NoRST, OB_RST_NoEN, OB_PowerON_Start_Mode_BOOT);
  FLASH_Lock();//**4232-4012=200�ֽ�

  /*********************Ӧ�ú�����ʼ��******************************/
  My_GPIO_Init(); // IO�ڳ�ʼ��****4484-4232=252�ֽ�
  TIM1_Init(100, (SystemCoreClock / (100*PWM_FRE)) - 1, PWM_Duty);// ��Ļ�ı������  Ĭ�ϰٷְ�����******5076-4484=592�ֽ�
 // Encoder_Init(12, 1);    // ������������,����ֵΪ65535������Ƶ��1Ȧ12��****6040-5148=900�ֽ�
//  LCD_Init();             // ��ĻӲ����ʼ��****200�ֽ�


//  LCD_SHOW_API_INIT();    // ��Ļ����******
//  KEY_INIT();             // ȷ�ϰ����жϳ�ʼ��***
//  Battery_Init();         // ��ص�adc��ʼ��****
//  EXTI7_INT_INIT();       // �ⲿ���������жϣ���ʼ���*****
//  EXTI6_INT_INIT();       // �ⲿ���������жϣ�lora����Ϣ������*****
//  SX1278_Init(434);       // lora�ĳ�ʼ��*****
//  startup_animation();
//  IWDG_Feed_Init(IWDG_Prescaler_128, 10000); // 4�벻ι���͸�λ   ��Ƶʱ���ڲ�128khz����128=1000��1����1000����4000=4s****
//
//
//
//  while (1)
//  {
//
//    Delay_Ms(500);
//    IWDG_ReloadCounter(); // ι��
//
//    // Paint_DrawString(77, 0, "ac", &Font24_En, BLACK,WHITE,  '0');
//
//    show_battery(); // ��ص�����ʾ����
//    SX1278_test();  //  14548         80    1504   16132    3f04 newmsg.elf
//
//    if (precircle != circle || (precnt != TIM2->CNT)) // �б仯�Ͷ�
//    {
//      printf("Encoder position= %d circle %d step\r\n", TIM2->CNT, circle);
//      precircle = circle;
//      precnt = TIM2->CNT;
//      system_wokeup();
//      MOTOR_ON;
//      Delay_Ms(50);
//      MOTOR_OFF;
//    }
//  }
}
