
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
    IWDG_SetReload(rlr); // ���� IWDG ����װ��ֵ�����ֵ������ IWDG �ĳ�ʱʱ��
    IWDG_ReloadCounter(); // ���¼��� IWDG ����������ʹ���������µ���װ��ֵ��ʼ
    IWDG_Enable(); // ���� IWDG��ʹ�俪ʼ����
}

int main(void)
{
    // ������ʼ��
    char strBuf[6]; // ���ڴ洢ת������ַ����������Ҫ5���ַ���һ����ֹ����
    u8 lorabuf[21]; // lora��buf
    u8 res;         // �����ķ���
    u8 len;
    /*********************�������ݳ�ʼ��******************************/
    SystemCoreClockUpdate();   // ϵͳʱ��ˢ��
    USART_Printf_Init(115200); // ���ڳ�ʼ����Ҫ�ڴ�ӡǰ����Ȼ�Ῠ��
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    Delay_Init(); // ��ʱ��ʼ����Ҫ����ʱǰ����Ȼ�Ῠ��

    /*********************Ӧ�ú�����ʼ��******************************/
    IWDG_Feed_Init(IWDG_Prescaler_128, 4000); // 4�벻ι���͸�λ   ��Ƶʱ���ڲ�128khz����128=1000��1����1000����4000=4s
    My_GPIO_Init();                           // IO�ڳ�ʼ��
    EXTI6_INT_INIT();                         // �ⲿ���������жϣ�lora����Ϣ������
    PWM_Config(10000, 100);                   // ��Ļ�ı������  Ĭ�ϰٷְ�����
    Encoder_Init(65535, 1);                       // ������������,����ֵΪ65535������Ƶ��1Ȧ24����4��*6��
    LCD_Init();                               // ��ĻӲ����ʼ��
    Battery_Init();                           // ��ص�adc��ʼ��
    SX1278_Init(434);                         // lora�ĳ�ʼ��
                                              // ȱ�ٿ�������
    while (1)
    {

        printf("Feed dog\r\n");
        IWDG_ReloadCounter(); // Feed dog

        //  LCD_0in85_test();        // ��Ļ����

        for (int i = 0; i < 4; i++)
        {
            sprintf(strBuf, "%04d", BattaryBuf[i]);
            printf("%s\r\n", strBuf);
            Paint_DrawString_EN(10, 34, strBuf, &Font24, RED, CYAN);
        }

        if (SX1278_LoRaTxPacket(lorabuf, 10))
        {
            printf("TX fail \r\n");
        }

        res = SX1278_LoRaRxPacket(lorabuf, &len, 3000);
        if (res == 0)
        {
            printf("RX sucess \r\n");
            Delay_Ms(500);
        }
        else if (res == 1)
        {
            printf("Time out!\r\n");
        }
        else if (res == 2)
        {
            printf("CRC eeror!\r\n");
        }

        printf("�ȵȽ���\r\n");
        LCD_0IN85_SetBackLight(100);

        Delay_Ms(1000);
        printf("�ȵȽ���\r\n");
        LCD_0IN85_SetBackLight(50);
        Delay_Ms(1000);

        if (precircle != circle || (precnt != TIM2->CNT && TIM2->CNT % 4 == 0))
        {
            printf("Encoder position= %d circle %d step\r\n", circle, TIM2->CNT);
            if (encodetime != 0)
                printf("Encoder speed= %f\r\n", -(float)(precircle * 80 + precnt - (circle * 80 + TIM2->CNT)) / (float)encodetime * 1000.0 / (float)SpeedSampleTimeMs / 80.0);
            else
                printf("Encoder speed null!!\r\n");
            encodetime = 0;
            precircle = circle;
            precnt = TIM2->CNT;
        }
        encodetime++;
        Delay_Ms(SpeedSampleTimeMs);
    }
}
