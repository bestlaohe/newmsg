
#include "debug.h"
#include "encode.h" // timeapp->encode->main
#include "adc.h"    // timeapp->pwm.screen->screen_api->adc->main
#include "SX1278.h" // gpio->SX1278->page
#include "page.h"

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
#define PWM_Duty 50

// 定义按键事件
typedef enum
{
  PAGE_SEND, // 发送页面
  PAGE_HISTROY_CHAT,
  PAGE_PERPARE_SETTING, // 准备设置页面

  PAGE_SETTING, // 设置页面
  PAGE_INFO,    // 信息页面
} Page;

Page page = PAGE_SEND;

int main(void)
{

  /*********************基本内容初始化******************************/
  SystemCoreClockUpdate();   // 48000000系统时钟刷新3324-3212=100k
  USART_Printf_Init(115200); // 串口初始化需要在打印前，不然会卡死3956-3324=600k
  printf("\r\n\r\n\r\n\r\nSystemClk:%d\r\n", SystemCoreClock);
  //  printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
  Delay_Init(); // 延时初始化需要在延时前，不然会卡死4012-3956=100字节
  FLASH_Unlock();
  FLASH_UserOptionByteConfig(OB_IWDG_SW, OB_STDBY_NoRST, OB_RST_NoEN, OB_PowerON_Start_Mode_BOOT);
  FLASH_Lock(); //**4232-4012=200字节
  /*********************应用函数初始化******************************/
  My_GPIO_Init();                                                    // IO口初始化****4484-4232=252字节
  TIM1_Init(100, (SystemCoreClock / (100 * PWM_FRE)) - 1, PWM_Duty); // 屏幕的背光调节  默认百分百亮度******5076-4484=592字节pwm要200多+定时器300
  TIM2_Init(11, 1);                                                  // 编码器的内容,重载值为65535，不分频，1圈12个****6020-6900=880字节输入捕获要500多+定时器300
  LCD_Drive_Init();                                                  // 屏幕硬件初始化****200字节
  LCD_SHOW_API_INIT();                                               // 屏幕测试******8404-6224=2180
  EXTI_INT_INIT();                                                   // 按键，充电，lora中断初始化
  Battery_Init();                                                    // 电池的adc初始化****9456-8636=820
  SX1278_Init(434);                                                  // lora的初始化*****10268-9620=648
 // startup_animation();                                               // 11732-10512=500
  IWDG_Feed_Init(IWDG_Prescaler_128, 10000);                         // 4秒不喂狗就复位   低频时钟内部128khz除以128=1000，1除以1000乘以4000=4s****12467-12356=111字节

  Delay_Ms(500);


  while (1)
  {
      //LCD_0IN85_SetBackLight(50);
//     Paint_DrawLine(20, 55, 20, 75, BLUE, 1, LINE_STYLE_SOLID);
//      Paint_DrawRectangle(0, 0, 50, 50, BLUE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
//     Paint_DrawRectangle(0, 100, 127, 127, RED, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

    switch (page) // 处理页面
    {
    case PAGE_SEND: // 发送界面

       chat_page(&Font8_En);

      if (key.event == KEY_EVENT_LONG_CLICK) // 返回

        page = PAGE_HISTROY_CHAT;

      break;

    case PAGE_HISTROY_CHAT: // 聊天记录界面

      chat_history_page();

      if (key.event == KEY_EVENT_LONG_CLICK)
        page = PAGE_PERPARE_SETTING;
      break;

    case PAGE_PERPARE_SETTING: // 准备设置界面

      Paint_DrawRectangle(0, 20, 127, 100, RED, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      Paint_DrawRectangle(0, 102, 127, 127, BLUE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      Paint_DrawChar(0, 0, 0, &Font16_Operate, GREEN, BLUE,0);
      if (key.event == KEY_EVENT_CLICK)
      {
        LCD_0IN85_Clear(BLACK);
        page = PAGE_SETTING;
      }

      if (key.event == KEY_EVENT_LONG_CLICK)
        page = PAGE_SEND;
      break;

    case PAGE_SETTING: // 设置界面

       Paint_DrawChar(40, 40, 0, &Font24_icon, BLACK, BLUE,0);

      if (key.event == KEY_EVENT_LONG_CLICK)
      {
        LCD_0IN85_Clear(BLACK);
        page = PAGE_INFO;
      }
      break;

    case PAGE_INFO: // 信息界面

     // info_page();

      if (key.event == KEY_EVENT_LONG_CLICK)
        page = PAGE_SEND;
      break;

    default: // 默认情况

      page = PAGE_SEND;
      break;
    }

    // 处理完事件后清除事件
    key.event = KEY_EVENT_NONE;
    encode.state = ENCODE_EVENT_NONE;

    //   Paint_DrawString(77, 0, "ac", &Font24_En, BLACK, WHITE, '0'); // 14272-11732=2540
    // Paint_DrawString(77, 0, "ac", &Font8_En, BLACK, WHITE, '0'); // 12608-11732=876

    // SX1278_test(); //  16180-15028=1652             会减少368
    Encoder_Scan();
    IWDG_ReloadCounter(); // 喂狗* 12484-12467=24字节
    Delay_Ms(10);
  }
}
