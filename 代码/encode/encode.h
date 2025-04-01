/*
 * encode.h
 *
 *  Created on: 2024年9月1日
 *      Author: 12630
 */

#ifndef ENCODE_ENCODE_H_
#define ENCODE_ENCODE_H_

#include "time_app.h"

extern Encode encode_struct;
extern volatile int circle;

void Encoder_Init(u16 arr, u16 psc);

void Encoder_Scan();

#endif /* ENCODE_ENCODE_H_ */
