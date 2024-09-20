
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

// 待机功耗最低，睡眠功耗其次
// 16k=16,384
// data是表示代码段（text segment）的大小，以字节为单位。代码段包含了程序的可执行指令。
// bss是存储那些在源代码中没有显式初始化的全局变量和静态变量。
// data是显式初始化的全局变量和静态变量

//  ROM：
//  text 段：存储程序代码。
//  data 段：存储已初始化的全局变量和静态变量的初始值。

//  RAM：
//  text 段：在运行时用于存储代码执行（不直接占用 RAM 的存储空间，但需要 RAM 来执行）。
//  data 段：在运行时存储已初始化的全局变量和静态变量。
//  bss 段：在运行时存储未初始化的全局变量和静态变量。

#define PWM_FRE 10000
#define PWM_Duty 100

// 定义按键事件
typedef enum
{
  PAGE_SEND,    // 发送页面
  PAGE_SETTING, // 设置页面
  PAGE_INFO,    // 信息页面
} Page;

Page page = PAGE_SEND;

extern Key key;
int main(void)
{
  uint16_t precnt = 0;
  int precircle = 0;

  /*********************基本内容初始化******************************/
  SystemCoreClockUpdate();   // 48000000系统时钟刷新3324-3212=100k
  USART_Printf_Init(115200); // 串口初始化需要在打印前，不然会卡死3956-3324=600k
  printf("SystemClk:%d\r\n", SystemCoreClock);
  //  printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
  Delay_Init(); // 延时初始化需要在延时前，不然会卡死4012-3956=100字节
  FLASH_Unlock();
  FLASH_UserOptionByteConfig(OB_IWDG_SW, OB_STDBY_NoRST, OB_RST_NoEN, OB_PowerON_Start_Mode_BOOT);
  FLASH_Lock(); //**4232-4012=200字节
  /*********************应用函数初始化******************************/
  My_GPIO_Init();                                                    // IO口初始化****4484-4232=252字节
  TIM1_Init(100, (SystemCoreClock / (100 * PWM_FRE)) - 1, PWM_Duty); // 屏幕的背光调节  默认百分百亮度******5076-4484=592字节pwm要200多+定时器300
  TIM2_Init(12, 1);                                                  // 编码器的内容,重载值为65535，不分频，1圈12个****6020-6900=880字节输入捕获要500多+定时器300
  LCD_Drive_Init();                                                  // 屏幕硬件初始化****200字节
  LCD_SHOW_API_INIT();                                               // 屏幕测试******8404-6224=2180
  KEY_INIT();                                                        // 确认按键中断初始化***8636-8404=232
  Battery_Init();                                                    // 电池的adc初始化****9456-8636=820
  EXTI7_INT_INIT();                                                  // 外部引进触发中断，开始充电*****9540=100
  EXTI6_INT_INIT();                                                  // 外部引进触发中断，lora有信息过来了*****9620=100
  SX1278_Init(434);                                                  // lora的初始化*****10268-9620=648
                                                                     // startup_animation();                                               // 11732-10512=1220
  IWDG_Feed_Init(IWDG_Prescaler_128, 10000);                         // 4秒不喂狗就复位   低频时钟内部128khz除以128=1000，1除以1000乘以4000=4s****12467-12356=111字节

  while (1)
  {

    // 处理页面
    switch (page)
    {
    case PAGE_SEND: // 发送界面

      show_battery(); // 电池电量显示出来16124-15028=1612
      Paint_DrawRectangle(0, 20, 128, 120,BLUE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      
      if (key->event = KEY_EVENT_HOLD)
        page = PAGE_SETTING;
      break;

    case PAGE_SETTING: // 设置界面

      if (key->event = KEY_EVENT_HOLD)
        page = PAGE_INFO;
      break;

    case PAGE_INFO: // 信息界面

      if (key->event = KEY_EVENT_HOLD)
        page = PAGE_SEND;
      break;

    default: // 默认情况

      page = PAGE_SEND;
      break;
    }

    // 处理完事件后清除事件
    key->event = KEY_EVENT_NONE;

    IWDG_ReloadCounter(); // 喂狗* 12484-12467=24字节

    //   Paint_DrawString(77, 0, "ac", &Font24_En, BLACK, WHITE, '0'); // 14272-11732=2540
    // Paint_DrawString(77, 0, "ac", &Font8_En, BLACK, WHITE, '0'); // 12608-11732=876

    //  SX1278_test(); //  16180-15028=1652             会减少368

    //    if (precircle != circle || (precnt != TIM2->CNT)) // 有变化就动   140字节
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
