#ifndef __WAVE_WHEEL_H__
#define __WAVE_WHEEL_H__

#include <stdint.h>
#include <stdbool.h>
#include <ch32v00x_it.h>


extern Encode encode;
/**
 * @brief 波动滚轮初始化
 * @param none
 */
void WaveWheel_Init(void);

/**
 * @brief 波动滚轮任务函数（需周期调用）
 * 建议每 10~20ms 调用一次
 */
void WaveWheel_Task(void);


#endif
