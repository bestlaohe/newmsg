/*
 * encode.c
 *
 *  Created on: 2024��9��1��
 *      Author: 12630
 */



/**
 * @note
 * Encoder rourine
 * The main function initializes and tests an encoder, and continuously prints the encoder position and
 * speed.
 */
#include "debug.h"
#include "encode.h"

void Encoder_Init(u16 arr, u16 psc){
    TIM2_Init(arr,psc);//�Զ���װ��ֵ��ARR����Ԥ��Ƶ����PSC��
}
