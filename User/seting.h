/*
 * seting.h
 *
 *  Created on: 2024��11��4��
 *      Author: heyj
 */

#ifndef USER_SETING_H_
#define USER_SETING_H_

#define THEME 0

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

#endif /* USER_SETING_H_ */
