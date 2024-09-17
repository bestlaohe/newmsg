
#include "debug.h"
#include "encode.h"
#include "adc.h"
#include "SX1278.h"

volatile int circle = 0, precircle = 0;
volatile uint16_t precnt = 0;
u16 BattaryBuf[10];

//******************guipain
//**********************|
//**********************|
//timeapp->pwm->screen->screen_api->adc->main
//timeapp->encode->main
//gpio->SX1278->main


// ����������ͣ�˯�߹������

void startup_animation() {

    int center_x = LCD_HEIGHT / 2;
    int center_y = LCD_WIDTH / 2;

    int initial_size = 1;   // ��ʼͼ���С
    int final_size = 48;    // ����ͼ���С
    int step = 2;           // ÿ�ηŴ󲽳�

    for (int size = initial_size; size <= final_size; size += step) {

     //  Paint_Clear(BLACK);
     //  Paint_Clear(GREEN);

        // ����ͼ�����Ͻ�λ�ã�ʹ�����ı�������Ļ����
        int icon_x = center_x - size / 2;
        int icon_y = center_y - size / 2;

        // ����ͼ��
        Delay_Ms(1);
        // �ӳ��Դ�������Ч��
       // Paint_Clear(RED);
        Paint_Drawicon(icon_x, icon_y, 3, &Font24_icon, BLACK, WHITE);

       // Delay_Ms(100);

        Paint_Drawicon(icon_x, icon_y, 3, &Font24_icon, BLACK, BLACK);//ȥ����Ӱ
    }
   // Paint_Clear(BLACK);

    Paint_Drawicon(40, 40, 3, &Font24_icon, BLACK, WHITE);


}



int main(void)
{

    /*********************�������ݳ�ʼ��******************************/
    SystemCoreClockUpdate();   // 48000000ϵͳʱ��ˢ��
    USART_Printf_Init(115200); // ���ڳ�ʼ����Ҫ�ڴ�ӡǰ����Ȼ�Ῠ��
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    Delay_Init(); // ��ʱ��ʼ����Ҫ����ʱǰ����Ȼ�Ῠ��

    /*********************Ӧ�ú�����ʼ��******************************/
   // IWDG_Feed_Init(IWDG_Prescaler_128, 4000); // 4�벻ι���͸�λ   ��Ƶʱ���ڲ�128khz����128=1000��1����1000����4000=4s****
    My_GPIO_Init();                           // IO�ڳ�ʼ��****
    MOTOR_OFF;
    PWM_Config(10000, 100);                   // ��Ļ�ı������  Ĭ�ϰٷְ�����******
   // Encoder_Init(12, 1);                      // ������������,����ֵΪ65535������Ƶ��1Ȧ12��****
    LCD_Init();                               // ��ĻӲ����ʼ��****
    LCD_SHOW_API_INIT();                      // ��Ļ����******
    KEY_INIT();                               // ȷ�ϰ����жϳ�ʼ��***
 //   Battery_Init();                           // ��ص�adc��ʼ��****
 //   EXTI6_INT_INIT();                         // �ⲿ���������жϣ�lora����Ϣ������*****
  //  SX1278_Init(434);                         // lora�ĳ�ʼ��*****


Delay_Ms(100);

  //Paint_Drawicon(111, 104, 3,&Font24_Num,  BLACK, WHITE);   //  ��������


      //  LCD_0IN85_SetWindows(dmaXpoint , dmaYpoint , dmaXpoint +dmaFont->Width - 1, dmaYpoint + dmaFont->Height - 1);
//    Paint_Clear(RED);
//
//
//    Paint_Clear(GREEN);
 //  Delay_Ms(100);
 //  Paint_Clear(RED);
  // Paint_Drawicon(40, 40, 3, &Font24_icon, BLACK, WHITE);

//      Delay_Ms(500);
 //     Paint_Clear(GREEN);
//      Delay_Ms(500);
//
//      Paint_Clear(RED);
//
//        Delay_Ms(500);
//        Paint_Clear(GREEN);
//        Delay_Ms(500);
  startup_animation();
    while (1)
    {

        Delay_Ms(500);
        IWDG_ReloadCounter(); // ι��
//        show_battery();//��ص�����ʾ����
 //       SX1278_test();
//
//
////
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
