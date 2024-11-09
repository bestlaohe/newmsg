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
#define PWM_Duty        50

#define ADC_CONUT        5//最大允许64，因为u16/1024=64

// 去抖动和长按检测的常数
#define DEBOUNCE_TIME 50 // 去抖动时间，单位：ms
#define HOLD_TIME 3000   // 长按时间，单位：100us



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


#endif /* USER_SETING_H_ */
