/*
 * encode.h
 *
 *  Created on: 2024��9��1��
 *      Author: 12630
 */

#ifndef ENCODE_ENCODE_H_
#define ENCODE_ENCODE_H_
#include "time_app.h"
#include "gpio.h"

void Encoder_Init(u16 arr, u16 psc);
u8 KEY_PRESS(void);
void KEY_INIT(void);
#endif /* ENCODE_ENCODE_H_ */
