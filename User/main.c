
 #include "debug.h"
 #include "string.h"
 #include "pwm.h"
 #include "drive_screen.h"
 #include "screen_api.h"
 #include "encode.h"
 #include "gpio.h"

 volatile int circle = 0, precircle = 0;
 volatile uint16_t precnt = 0;
 volatile uint32_t time = 0;


void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
* @fn TIM1_UP_IRQHandler
*
* @brief TIM1_UP_IRQHandler function handles the interrupt for TIM1 and updates the "circle" variable based
* on the current count and auto-reload values of TIM1.
*/
void TIM1_UP_IRQHandler()
{

  volatile uint16_t tempcnt = TIM1->CNT, temparr = TIM1->ATRLR;
  if (TIM_GetITStatus(TIM1, TIM_IT_Update))
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
  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
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
    DEV_Module_Init();
    LCD_0in85_test();

    // TIM1_Encoder_Init();

     while (1)
     {

         printf("等等结束\r\n");
           LCD_0IN85_SetBackLight(100);

             Delay_Ms(1000);
             printf("等等结束\r\n");
                 LCD_0IN85_SetBackLight(50);
                 Delay_Ms(1000);

         /* The code block you provided is checking if the encoder position or count has changed. If
         there is a change, it prints the new encoder position and calculates the encoder speed. */
 //        if (precircle != circle || (precnt != TIM1->CNT && TIM1->CNT % 4 == 0))
 //        {
 //            printf("Encoder position= %d circle %d step\r\n", circle, TIM1->CNT);
 //            if (time != 0)
 //                printf("Encoder speed= %f\r\n", -(float)(precircle * 80 + precnt - (circle * 80 + TIM2->CNT)) / (float)time * 1000.0/(float)SpeedSampleTimeMs / 80.0);
 //            else
 //                printf("Encoder speed null!!\r\n");
 //            time = 0;
 //            precircle = circle;
 //            precnt = TIM1->CNT;
 //        }
 //        time++;
 //        Delay_Ms(SpeedSampleTimeMs);
     }
 }





