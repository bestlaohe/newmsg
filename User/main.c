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

// 似乎休眠没喂狗重启了，加喂狗打印
int main(void)
{

  /*********************基本内容初始化******************************/
  SystemCoreClockUpdate();   // 48000000系统时钟刷新3324-3212=100k
  USART_Printf_Init(115200); // 串口初始化需要在打印前，不然会卡死3956-3324=600k
  DEBUG_PRINT("\r\n\r\n\r\n\r\nSystemClk:%d\r\n", SystemCoreClock);
  //  DEBUG_PRINT("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
  Delay_Init(); // 延时初始化需要在延时前，不然会卡死4012-3956=100字节
  FLASH_Unlock();
  FLASH_UserOptionByteConfig(OB_IWDG_SW, OB_STDBY_NoRST, OB_RST_NoEN, OB_PowerON_Start_Mode_BOOT);
  FLASH_Lock(); //**4232-4012=200字节
  /*********************应用函数初始化******************************/
  Check_Reset_Flag();                                                // 查询复位原因
  My_GPIO_Init();                                                    // IO口初始化****4484-4232=252字节
  TIM1_Init(100, (SystemCoreClock / (100 * PWM_FRE)) - 1, PWM_Duty); // 屏幕的背光调节  默认百分百亮度******5076-4484=592字节pwm要200多+定时器300
  TIM2_Init(11, 1);                                                  // 编码器的内容,重载值为65535，不分频，1圈12个****6020-6900=880字节输入捕获要500多+定时器300
  LCD_Drive_Init();                                                  // 屏幕硬件初始化****200字节
  LCD_SHOW_API_INIT();                                               // 屏幕测试******8404-6224=2180
  Battery_Init();                                                    // 电池的adc初始化****9456-8636=820
  SX1278_Init();                                                     // lora的初始化*****10268-9620=648
  EXTI_INT_INIT();                                                   // 按键，充电，lora中断初始化
  // startup_animation();                                             // 开机动画
  IWDG_Feed_Init(IWDG_Prescaler_256, 4000); // 该参数必须是介于 0 和 0x0FFF 之间的一个数值    // 4秒不喂狗就复位   低频时钟内部128khz除以256=500，1除以500乘以4000=8s****12467-12356=111字节

  // 测试代码
  LCD_0IN85_Clear(MY_THEME_BACK_COLOR);
  //  show_battery(97, 3, MY_THEME_BACK_COLOR, MY_THEME_COMPONT_COLOR);                         // 显示电池信息
  // Paint_DrawChar(1, 1, 0, &Font16_Operate, MY_THEME_BACK_COLOR, MY_THEME_COMPONT_COLOR, 0); // 设置的图标
  //
  //  // 测试代码
  //  while (1)
  //    {}
  while (1)
  {

    show_page();
    SX1278_Receive();
    Encoder_Scan();
    IWDG_ReloadCounter(); // 喂狗
    // DEBUG_PRINT("HEART\r\n");
    // Delay_Ms(200);
  }
}
