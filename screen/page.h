/*
 * page.h
 *
 *  Created on: 2024Äê9ÔÂ21ÈÕ
 *      Author: 12630
 */

#ifndef SCREEN_PAGE_H_
#define SCREEN_PAGE_H_
#include "debug.h"
#include "encode.h"
#include "adc.h"
//#include "SX1278.h"



extern Key key;

extern Encode encode;


extern char lora_receive_buf[50];



void chat_page(sFONT *Font);
void show_history_data();
void chat_history_page();
void info_page();
void setting_page();
#endif /* SCREEN_PAGE_H_ */
