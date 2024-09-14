
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



// 待机功耗最低，睡眠功耗其次
int main(void)
{
    // 变量初始化
    char strBuf[6]; // 用于存储转换后的字符串（最多需要5个字符加一个终止符）

    /*********************基本内容初始化******************************/
    SystemCoreClockUpdate();   // 24mhz系统时钟刷新
    USART_Printf_Init(115200); // 串口初始化需要在打印前，不然会卡死
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    Delay_Init(); // 延时初始化需要在延时前，不然会卡死

    /*********************应用函数初始化******************************/
    IWDG_Feed_Init(IWDG_Prescaler_128, 4000); // 4秒不喂狗就复位   低频时钟内部128khz除以128=1000，1除以1000乘以4000=4s
    My_GPIO_Init();                           // IO口初始化****
    EXTI6_INT_INIT();                         // 外部引进触发中断，lora有信息过来了
    PWM_Config(10000, 100);                   // 屏幕的背光调节  默认百分百亮度******
    Encoder_Init(12, 1);                      // 编码器的内容,重载值为65535，不分频，1圈24个，4倍*6格
    LCD_Init();                               // 屏幕硬件初始化****
    LCD_SHOW_API_INIT();                      // 屏幕测试
    Battery_Init();                           // 电池的adc初始化
    SX1278_Init(434);                         // lora的初始化
    KEY_INIT();                               // 确认按键中断初始化
                                              //  缺少开机界面

    while (1)
    {

        Delay_Ms(500);
        IWDG_ReloadCounter(); // 喂狗
        SX1278_test();

        for (int i = 0; i < 10; i++)
        {

            printf(" Battery Percentage: %u%%\n", get_battery_percentage(BattaryBuf[i]));
            sprintf(strBuf, "%04d", get_battery_percentage(BattaryBuf[i]));
            printf("电池百分比值为%s\r\n", strBuf);
            Paint_DrawString(10, 34, strBuf, &Font24_Num, RED, CYAN,'/');
            Paint_DrawString(10, 34, strBuf, &Font24_En, RED, CYAN,'`');//数字的偏移为'/'，英文的偏移为'`'
        }

        if (precircle != circle || (precnt != TIM2->CNT)) // 有变化就动
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
