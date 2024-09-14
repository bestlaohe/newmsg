
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
    // ������ʼ��
    char strBuf[6]; // ���ڴ洢ת������ַ����������Ҫ5���ַ���һ����ֹ����

    /*********************�������ݳ�ʼ��******************************/
    SystemCoreClockUpdate();   // 24mhzϵͳʱ��ˢ��
    USART_Printf_Init(115200); // ���ڳ�ʼ����Ҫ�ڴ�ӡǰ����Ȼ�Ῠ��
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    Delay_Init(); // ��ʱ��ʼ����Ҫ����ʱǰ����Ȼ�Ῠ��

    /*********************Ӧ�ú�����ʼ��******************************/
    IWDG_Feed_Init(IWDG_Prescaler_128, 4000); // 4�벻ι���͸�λ   ��Ƶʱ���ڲ�128khz����128=1000��1����1000����4000=4s
    My_GPIO_Init();                           // IO�ڳ�ʼ��****
    EXTI6_INT_INIT();                         // �ⲿ���������жϣ�lora����Ϣ������
    PWM_Config(10000, 100);                   // ��Ļ�ı������  Ĭ�ϰٷְ�����******
    Encoder_Init(12, 1);                      // ������������,����ֵΪ65535������Ƶ��1Ȧ24����4��*6��
    LCD_Init();                               // ��ĻӲ����ʼ��****
    LCD_SHOW_API_INIT();                      // ��Ļ����
    Battery_Init();                           // ��ص�adc��ʼ��
    SX1278_Init(434);                         // lora�ĳ�ʼ��
    KEY_INIT();                               // ȷ�ϰ����жϳ�ʼ��
                                              //  ȱ�ٿ�������

    while (1)
    {

        Delay_Ms(500);
        IWDG_ReloadCounter(); // ι��
        SX1278_test();

        for (int i = 0; i < 10; i++)
        {

            printf(" Battery Percentage: %u%%\n", get_battery_percentage(BattaryBuf[i]));
            sprintf(strBuf, "%04d", get_battery_percentage(BattaryBuf[i]));
            printf("��ذٷֱ�ֵΪ%s\r\n", strBuf);
            Paint_DrawString(10, 34, strBuf, &Font24_Num, RED, CYAN,'/');
            Paint_DrawString(10, 34, strBuf, &Font24_En, RED, CYAN,'`');//���ֵ�ƫ��Ϊ'/'��Ӣ�ĵ�ƫ��Ϊ'`'
        }

        if (precircle != circle || (precnt != TIM2->CNT)) // �б仯�Ͷ�
        {
            printf("Encoder position= %d circle %d step\r\n", TIM2->CNT, circle);
            precircle = circle;
            precnt = TIM2->CNT;
            system_wokeup();
            MOTOR_ON;
            Delay_Ms(10);
            MOTOR_OFF;
        }
    }
}
