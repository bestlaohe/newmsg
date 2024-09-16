
#include "debug.h"
#include "string.h"
#include "pwm.h"
#include "drive_screen.h"
#include "screen_api.h"
#include "encode.h"
#include "gpio.h"
#include "adc.h"
#include "SX1278.h"

volatile int circle = 0, precircle = 0;
volatile uint16_t precnt = 0;
u16 BattaryBuf[10];



// ����������ͣ�˯�߹������
int main(void)
{

    /*********************�������ݳ�ʼ��******************************/
    SystemCoreClockUpdate();   // 24mhzϵͳʱ��ˢ��
    USART_Printf_Init(115200); // ���ڳ�ʼ����Ҫ�ڴ�ӡǰ����Ȼ�Ῠ��
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    Delay_Init(); // ��ʱ��ʼ����Ҫ����ʱǰ����Ȼ�Ῠ��

    /*********************Ӧ�ú�����ʼ��******************************/
  //  IWDG_Feed_Init(IWDG_Prescaler_128, 4000); // 4�벻ι���͸�λ   ��Ƶʱ���ڲ�128khz����128=1000��1����1000����4000=4s****
    My_GPIO_Init();                           // IO�ڳ�ʼ��****
    MOTOR_OFF;
    PWM_Config(10000, 100);                   // ��Ļ�ı������  Ĭ�ϰٷְ�����******
    Encoder_Init(12, 1);                      // ������������,����ֵΪ65535������Ƶ��1Ȧ12��****
    LCD_Init();                               // ��ĻӲ����ʼ��****
    LCD_SHOW_API_INIT();                      // ��Ļ����******
    KEY_INIT();                               // ȷ�ϰ����жϳ�ʼ��***
    Battery_Init();                           // ��ص�adc��ʼ��****
    EXTI6_INT_INIT();                         // �ⲿ���������жϣ�lora����Ϣ������*****

    SX1278_Init(434);                         // lora�ĳ�ʼ��


    Paint_Drawicon(40, 40, 3,&Font24_icon,  BLACK, WHITE);   //  ��������

    while (1)
    {

        Delay_Ms(500);
        IWDG_ReloadCounter(); // ι��
      // show_battery();//��ص�����ʾ����
       SX1278_test();


//
//        if (precircle != circle || (precnt != TIM2->CNT)) // �б仯�Ͷ�
//        {
//            printf("Encoder position= %d circle %d step\r\n", TIM2->CNT, circle);
//            precircle = circle;
//            precnt = TIM2->CNT;
//            system_wokeup();
//            MOTOR_ON;
//            Delay_Ms(50);
//            MOTOR_OFF;
//        }
    }
}
