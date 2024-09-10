
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
 * @brief   初始化 IWDG（独立看门狗）。
 *
 * @param   prer: 指定 IWDG 的预分频器值。
 *            IWDG_Prescaler_4: IWDG 预分频器设置为 4。
 *            IWDG_Prescaler_8: IWDG 预分频器设置为 8。
 *            IWDG_Prescaler_16: IWDG 预分频器设置为 16。
 *            IWDG_Prescaler_32: IWDG 预分频器设置为 32。
 *            IWDG_Prescaler_64: IWDG 预分频器设置为 64。
 *            IWDG_Prescaler_128: IWDG 预分频器设置为 128。
 *            IWDG_Prescaler_256: IWDG 预分频器设置为 256。
 *          rlr: 指定 IWDG 的重装载值。
 *            该参数必须是介于 0 和 0x0FFF 之间的一个数值。
 *
 * @return  none
 */
void IWDG_Feed_Init(u16 prer, u16 rlr)
{

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); // 使能 IWDG 写访问，以便可以设置 IWDG 的配置寄存器
    IWDG_SetPrescaler(prer);                      // 设置 IWDG 的预分频器值
    IWDG_SetReload(rlr); // 设置 IWDG 的重装载值，这个值决定了 IWDG 的超时时间
    IWDG_ReloadCounter(); // 重新加载 IWDG 计数器，以使计数器从新的重装载值开始
    IWDG_Enable(); // 启用 IWDG，使其开始工作
}

int main(void)
{
    // 变量初始化
    char strBuf[6]; // 用于存储转换后的字符串（最多需要5个字符加一个终止符）
    u8 lorabuf[21]; // lora的buf
    u8 res;         // 操作的返回
    u8 len;
    /*********************基本内容初始化******************************/
    SystemCoreClockUpdate();   // 系统时钟刷新
    USART_Printf_Init(115200); // 串口初始化需要在打印前，不然会卡死
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    Delay_Init(); // 延时初始化需要在延时前，不然会卡死

    /*********************应用函数初始化******************************/
    IWDG_Feed_Init(IWDG_Prescaler_128, 4000); // 4秒不喂狗就复位   低频时钟内部128khz除以128=1000，1除以1000乘以4000=4s
    My_GPIO_Init();                           // IO口初始化
    EXTI6_INT_INIT();                         // 外部引进触发中断，lora有信息过来了
    PWM_Config(10000, 100);                   // 屏幕的背光调节  默认百分百亮度
    Encoder_Init(65535, 1);                       // 编码器的内容,重载值为65535，不分频，1圈24个，4倍*6格
    LCD_Init();                               // 屏幕硬件初始化
    Battery_Init();                           // 电池的adc初始化
    SX1278_Init(434);                         // lora的初始化
                                              // 缺少开机界面
    while (1)
    {

        printf("Feed dog\r\n");
        IWDG_ReloadCounter(); // Feed dog

        //  LCD_0in85_test();        // 屏幕测试

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

        printf("等等结束\r\n");
        LCD_0IN85_SetBackLight(100);

        Delay_Ms(1000);
        printf("等等结束\r\n");
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
