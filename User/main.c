
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
volatile uint32_t encodetime = 0;
u16 BattaryBuf[10];

#define SpeedSampleTimeMs 20

/*********************************************************************
 * @fn      IWDG_Feed_Init
 *
 * @brief   ��ʼ�� IWDG���������Ź�����
 *
 * @param   prer: ָ�� IWDG ��Ԥ��Ƶ��ֵ��
 *            IWDG_Prescaler_4: IWDG Ԥ��Ƶ������Ϊ 4��
 *            IWDG_Prescaler_8: IWDG Ԥ��Ƶ������Ϊ 8��
 *            IWDG_Prescaler_16: IWDG Ԥ��Ƶ������Ϊ 16��
 *            IWDG_Prescaler_32: IWDG Ԥ��Ƶ������Ϊ 32��
 *            IWDG_Prescaler_64: IWDG Ԥ��Ƶ������Ϊ 64��
 *            IWDG_Prescaler_128: IWDG Ԥ��Ƶ������Ϊ 128��
 *            IWDG_Prescaler_256: IWDG Ԥ��Ƶ������Ϊ 256��
 *          rlr: ָ�� IWDG ����װ��ֵ��
 *            �ò��������ǽ��� 0 �� 0x0FFF ֮���һ����ֵ��
 *
 * @return  none
 */
void IWDG_Feed_Init(u16 prer, u16 rlr)
{

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); // ʹ�� IWDG д���ʣ��Ա�������� IWDG �����üĴ���
    IWDG_SetPrescaler(prer);                      // ���� IWDG ��Ԥ��Ƶ��ֵ
    IWDG_SetReload(rlr);                          // ���� IWDG ����װ��ֵ�����ֵ������ IWDG �ĳ�ʱʱ��
    IWDG_ReloadCounter();                         // ���¼��� IWDG ����������ʹ���������µ���װ��ֵ��ʼ
    IWDG_Enable();                                // ���� IWDG��ʹ�俪ʼ����
}

int main(void)
{
    // ������ʼ��
    char strBuf[6]; // ���ڴ洢ת������ַ����������Ҫ5���ַ���һ����ֹ����
    u8 lorabuf[21]; // lora��buf
    u8 res;         // �����ķ���
    u8 len;
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
    Battery_Init();                           // ��ص�adc��ʼ��
    SX1278_Init(434);                         // lora�ĳ�ʼ��
    KEY_INIT();                               // ȷ�ϰ����жϳ�ʼ��
                                              //  ȱ�ٿ�������
                                              //  ȱ�����߹���  ����������ͣ�˯�߹�����Σ�˯�ߣ�����Ϣ�������ѣ����ֻ��߰�ť�������ѣ�10s������
    // 15s���ߣ�ÿ�ζ�ˢ�¶�ʱ��
  //  LCD_0in85_test(); // ��Ļ����
    while (1)
    {

        Delay_Ms(500);
        IWDG_ReloadCounter(); // ι��

        for (int i = 0; i <10 ; i++)
        {
            sprintf(strBuf, "%04d", BattaryBuf[i]);
            printf("�����ɼ�ֵΪ%d\r\n", BattaryBuf[i]);
            printf("��ѹΪ%d\r\n", (BattaryBuf[i] /1024) * 3);
            printf("��ѹΪ%d\r\n", (BattaryBuf[i] * 3 + 1024 / 2) / 1024);

            printf("adc�ɼ�ֵΪ%s\r\n", strBuf);


           // Paint_DrawString_EN(10, 34, strBuf, &Font24, RED, CYAN);
        }

//        if (SX1278_LoRaTxPacket(lorabuf, 10))
//        {
//            printf("TX fail \r\n");
//        }
//
//        res = SX1278_LoRaRxPacket(lorabuf, &len, 3000);
//        if (res == 0)
//        {
//            printf("RX sucess \r\n");
//            Delay_Ms(500);
//        }
//        else if (res == 1)
//        {
//            printf("Time out!\r\n");
//        }
//        else if (res == 2)
//        {
//            printf("CRC eeror!\r\n");
//        }

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
