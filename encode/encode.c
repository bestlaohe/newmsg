/*
 * encode.c
 *
 *  Created on: 2024年9月1日
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
    TIM2_Init(arr,psc);//自动重装载值（ARR）和预分频器（PSC）
}
