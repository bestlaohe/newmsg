
#include "debug.h"
#include "string.h"
#include "pwm.h"
#include "drive_screen.h"
#include "screen_api.h"

int main(void)
{

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    PWM_Config(10000, 100);

    DEV_Module_Init();
    LCD_0in85_test();

    while(1)
    {
        printf("等等结束\r\n");
        LCD_0IN85_SetBackLight(100);

          Delay_Ms(5000);
          printf("等等结束\r\n");
              LCD_0IN85_SetBackLight(50);


    }
}
