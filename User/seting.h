/*
 * seting.h
 *
 *  Created on: 2024��11��4��
 *      Author: heyj
 */

#ifndef USER_SETING_H_
#define USER_SETING_H_

#define THEME 1

#if THEME == 1 // ��ɫ����

#define MY_THEME_BACK_COLOR BLACK    // ������ɫ
#define MY_THEME_COMPONT_COLOR WHITE // �����ɫ
#define MY_SCREEN_COLOR MY_GRAY      // Ļ����ɫ


#else // ��ɫ����

#define MY_THEME_BACK_COLOR WHITE     // ������ɫ
#define MY_THEME_COMPONT_COLOR BLACK  // �����ɫ
#define MY_SCREEN_COLOR MY_WHITE_GRAY // Ļ����ɫ

#endif

// ����������Ŀ��
#define Font_WIDTH 7  // ������
#define Font_HEIGH 12 // ����߶�
#define BATTERY_X 97  // ���x����
#define BATTERY_Y 3   // ���y����

#define PWM_FRE         10000
#define PWM_Duty        50

#define ADC_CONUT        5//�������64����Ϊu16/1024=64

// ȥ�����ͳ������ĳ���
#define DEBOUNCE_TIME 50 // ȥ����ʱ�䣬��λ��ms
#define HOLD_TIME 3000   // ����ʱ�䣬��λ��100us



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
