/*
 * seting.h
 *
 *  Created on: 2024年11月4日
 *      Author: heyj
 */

#ifndef USER_SETING_H_
#define USER_SETING_H_

#define THEME 1

#if THEME == 1 // 黑色主题

#define MY_THEME_BACK_COLOR BLACK    // 背景颜色
#define MY_THEME_COMPONT_COLOR WHITE // 组件颜色
#define MY_SCREEN_COLOR MY_GRAY      // 幕布颜色


#else // 白色主题

#define MY_THEME_BACK_COLOR WHITE     // 背景颜色
#define MY_THEME_COMPONT_COLOR BLACK  // 组件颜色
#define MY_SCREEN_COLOR MY_WHITE_GRAY // 幕布颜色

#endif

// 主界面字体的宽高
#define Font_WIDTH 7  // 字体宽度
#define Font_HEIGH 12 // 字体高度
#define BATTERY_X 97  // 电池x坐标
#define BATTERY_Y 3   // 电池y坐标

#define PWM_FRE         10000
#define PWM_Duty        20      //初始占空比

#define ADC_CONUT        15//最大允许64，因为u16/1024=64

// 去抖动和长按检测的常数
#define DEBOUNCE_TIME 50 // 去抖动时间，单位：ms
#define HOLD_TIME 3000   // 长按时间，单位：100us

 #define SHAKE_TIME 50*10 // 大约50ms触发一次
 #define SLEEP_TIME 30000*10// 大约10s触发一次

 // 开启或关闭调试信息输出
 #define DEBUG_ENABLED 0

 #if DEBUG_ENABLED == 2
     #define DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
     #define SLEEP 0  //是否要休眠

 #elif DEBUG_ENABLED == 1
     #define DEBUG_PRINT(fmt, ...) my_uart_print(fmt)
     #define SLEEP 0   //是否要休眠
   
 #else
     #define DEBUG_PRINT(fmt, ...)
       #define SLEEP 1   //是否要休眠

 #endif




// struct Flags {
//     unsigned int refreshState : 1;
//     unsigned int flag2 : 1;
//     unsigned int flag3 : 1;
//     unsigned int flag4 : 1;
//     unsigned int flag5 : 1;
//     unsigned int flag6 : 1;
//     unsigned int flag7 : 1;
//     unsigned int flag8 : 1;
// };
// struct Flags myFlags = {0};

// 组件使能配置
// 1表示启用，0表示禁用
#define LORA_ENABLED     1  // LoRa组件使能
#define SCREEN_ENABLED   1  // 屏幕组件使能
#define ENCODER_ENABLED  1  // 编码器组件使能
#define BATTERY_ENABLED  1  // 电池组件使能

#endif /* USER_SETING_H_ */
