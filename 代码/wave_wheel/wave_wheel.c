#include "wave_wheel.h"
#include <stdio.h>
// 方案设计，用定时器去调用这个
//========================= 用户配置区 =========================//

// 上/下波动输入引脚读取函数（你需要根据实际IO修改）
#define WAVE_UP_READ() (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3))   // 举例
#define WAVE_DOWN_READ() (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4)) // 举例

// 波动电平逻辑（高电平触发拨动）
#define WAVE_ACTIVE_LEVEL 1

// 自加/自减的间隔（单位：ms）
#define BASE_TICKS 20   // 对应原来的 BASE_INTERVAL_MS
#define FAST_TICKS 4    // 对应原来的 FAST_INTERVAL_MS
#define ACCEL_TICKS 300 // 对应原来的 ACCEL_TIME_MS / 10ms
// 获取系统毫秒时间函数（你需要用 SysTick 实现）
extern uint32_t get_sys_time_ms(void);

//========================= 全局变量 =========================//

static uint32_t last_step_time = 0;
static uint32_t hold_start_time = 0;
static bool is_up = false;
static bool is_down = false;
static uint16_t hold_ticks = 0; // 按下保持计数
static uint16_t step_ticks = 0; // 自加步进计数
//========================= 内部函数 =========================//

void WaveWheel_Init(void)
{

    last_step_time = 0;
    hold_start_time = 0;
    is_up = false;
    is_down = false;
}

/**
 * @brief 波动滚轮任务函数
 * 每 10~20ms 调用一次
 */
void WaveWheel_Task(void)
{
    int up_state = WAVE_UP_READ();
    int down_state = WAVE_DOWN_READ();

    // 检测是否有拨动
    if ((up_state == WAVE_ACTIVE_LEVEL) || (down_state == WAVE_ACTIVE_LEVEL))
    {
        // 初次拨动
        if (!is_up && (up_state == WAVE_ACTIVE_LEVEL))
        {
            encode.state = ENCODE_EVENT_UP;
            hold_ticks = 0;
            step_ticks = 0;
            is_up = true;
        }
        else if (!is_down && (down_state == WAVE_ACTIVE_LEVEL))
        {
            encode.state = ENCODE_EVENT_DOWN;
            hold_ticks = 0;
            step_ticks = 0;
            is_down = true;
        }

        // 持续拨动计数
        hold_ticks++;

        // 根据保持时间选择步进间隔
        uint16_t interval = BASE_TICKS;
        if (hold_ticks >= ACCEL_TICKS)
            interval = FAST_TICKS;

        // 自加计数
        step_ticks++;
        if (step_ticks >= interval)
        {
            step_ticks = 0;
            if (is_up)
                encode.state = ENCODE_EVENT_UP;
            else if (is_down)
                encode.state = ENCODE_EVENT_DOWN;
        }
    }
    else
    {
        // 没有拨动，重置
        encode.state = ENCODE_EVENT_NONE;
        hold_ticks = 0;
        step_ticks = 0;
        is_up = false;
        is_down = false;
    }
}
