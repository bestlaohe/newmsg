
 #include "debug.h"
 #include "string.h"
 #include "pwm.h"
 #include "drive_screen.h"
 #include "screen_api.h"
 #include "encode.h"
 #include "gpio.h"

 volatile int circle = 0, precircle = 0;
 volatile uint16_t precnt = 0;
 volatile uint32_t encodetime = 0;


void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));




void TIM2_IRQHandler()
{

  volatile uint16_t tempcnt = TIM2->CNT, temparr = TIM2->ATRLR;
  if (TIM_GetITStatus(TIM2, TIM_IT_Update))
  {

      if (tempcnt < temparr / 2)
      {
          circle += 1;
      }
      else
      {
          circle -= 1;
      }
  }
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}
 #define SpeedSampleTimeMs 20

 int main(void)
 {
    SystemCoreClockUpdate();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    Delay_Init();
    My_GPIO_Init();

    PWM_Config(10000, 100);
    Encoder_Init(80,0);

    DEV_Module_Init();
    LCD_0in85_test();



     while (1)
     {

        printf("等等结束\r\n");
        LCD_0IN85_SetBackLight(100);

        Delay_Ms(1000);
        printf("等等结束\r\n");
        LCD_0IN85_SetBackLight(50);


         if (precircle != circle || (precnt != TIM2->CNT && TIM2->CNT % 4 == 0))
         {
             printf("Encoder position= %d circle %d step\r\n", circle, TIM2->CNT);
             if (encodetime != 0)
                 printf("Encoder speed= %f\r\n", -(float)(precircle * 80 + precnt - (circle * 80 + TIM2->CNT)) / (float)encodetime * 1000.0/(float)SpeedSampleTimeMs / 80.0);
             else
                 printf("Encoder speed null!!\r\n");
             encodetime = 0;
             precircle = circle;
             precnt = TIM2->CNT;
         }
         encodetime++;
         Delay_Ms(SpeedSampleTimeMs);
     }
 }





