#include "wave_wheel.h"

// 方案设计，用定时器去调用这个
//========================= 用户配置区 =========================//

// 上/下波动输入引脚读取函数（你需要根据实际IO修改）
#define WAVE_UP_READ() (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4))   // 举例
#define WAVE_DOWN_READ() (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3)) // 举例

// 波动电平逻辑（高电平触发拨动）
#define WAVE_ACTIVE_LEVEL 0

// 自加/自减的间隔（单位：ms）
#define BASE_TICKS 40   // 正常反应速度
#define FAST_TICKS 8    // 连按速度
#define ACCEL_TICKS 100 // 长按超过多少加速要乘以10ms一次
// 获取系统毫秒时间函数（你需要用 SysTick 实现）
extern uint32_t get_sys_time_ms(void);

//========================= 全局变量 =========================//

static uint32_t last_step_time = 0;
static uint32_t hold_start_time = 0;
static uint8_t is_up = 0;
static uint8_t is_down = 0;
static uint16_t hold_ticks = 0; // 按下保持计数
static uint16_t step_ticks = 0; // 自加步进计数
//========================= 内部函数 =========================//

void WaveWheel_Init(void)
{

    last_step_time = 0;
    hold_start_time = 0;
    is_up = 0;
    is_down = 0;
}

/**
 * @brief 波动滚轮任务函数
 * 每 10~20ms 调用一次
 */
void WaveWheel_Task(void)
{
    int up_state = WAVE_UP_READ();
    int down_state = WAVE_DOWN_READ();
    // uint16_t interval = BASE_TICKS;
    // 检测是否有拨动
    if ((up_state == WAVE_ACTIVE_LEVEL) || (down_state == WAVE_ACTIVE_LEVEL))
    {
        // 初次拨动
        if (!is_up && (up_state == WAVE_ACTIVE_LEVEL))
        {
            DEBUG_PRINT("Encoder up1\r\n");
            encode_struct.state = ENCODE_EVENT_UP;
            hold_ticks = 0;
            step_ticks = 0;
            is_up = 1;
        }
        else if (!is_down && (down_state == WAVE_ACTIVE_LEVEL))
        {
            DEBUG_PRINT("Encoder down1\r\n");
            encode_struct.state = ENCODE_EVENT_DOWN;
            hold_ticks = 0;
            step_ticks = 0;
            is_down = 1;
        }

        // 持续拨动计数
        hold_ticks++;

        // 根据保持时间选择步进间隔

        if (hold_ticks >= ACCEL_TICKS)
        {

            if (is_up)
            {
                hold_ticks = 0;
                encode_struct.state = ENCODE_EVENT_UP_LONG;
            }
            else if (is_down)
            {
                hold_ticks = 0;
                encode_struct.state = ENCODE_EVENT_DOWN_LONG;
            }
        }

        // 用于以后长安波动加速的设计
        //      interval = FAST_TICKS;

        // // 自加计数
        // step_ticks++;
        // if (step_ticks >= interval)
        // {
        //     step_ticks = 0;
        //     if (is_up)
        //     {
        //         DEBUG_PRINT("Encoder up2\r\n");
        //         encode_struct.state = ENCODE_EVENT_UP;
        //     }
        //     else if (is_down)
        //     {
        //         DEBUG_PRINT("Encoder down2\r\n");
        //         encode_struct.state = ENCODE_EVENT_DOWN;
        //     }
        // }
    }
    else
    {
        // 没有拨动，重置
        encode_struct.state = ENCODE_EVENT_NONE;
        hold_ticks = 0;
        step_ticks = 0;
        is_up = 0;
        is_down = 0;
    }
}
