
 #include "debug.h"
 #include "string.h"
 #include "pwm.h"
 #include "drive_screen.h"
 #include "screen_api.h"
 #include "encode.h"
 #include "gpio.h"
 #include "adc.h"
#include "SX1278.h"

 volatile int circle = 0, precircle = 0;
 volatile uint16_t precnt = 0;
 volatile uint32_t encodetime = 0;
 u16 TxBuf[10];



 #define SpeedSampleTimeMs 20

 int main(void)
 {
    SystemCoreClockUpdate();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    Delay_Init();
    My_GPIO_Init();
    EXTI6_INT_INIT();
    PWM_Config(10000, 100);
    Encoder_Init(80,0);

    DEV_Module_Init();
    LCD_0in85_test();
    ADC_Function_Init();

    DMA_Tx_Init(DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)TxBuf, 10);//电量会一直存在这个buf里面
    DMA_Cmd(DMA1_Channel1, ENABLE);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_241Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    Delay_Ms(50);
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);

    for(int i = 0; i < 10; i++){
    printf("%04d\r\n", TxBuf[i]);
        Delay_Ms(10);
    }
        u8 buf[21];
        u8 res;
        u8 len;
    SX1278_Init(434);


     while (1)
     {


//   if(SX1278_LoRaTxPacket(buf,10))
//         {
//             printf("TX fail \r\n");
//         }


//         res = SX1278_LoRaRxPacket(buf,&len,3000);
//         if(res == 0)
//         {
//             printf("RX sucess \r\n");
//             Delay_Ms(500);
//         }else if(res == 1)
//         {
//             printf("Time out!\r\n");
//         }else if(res == 2)
//         {
//             printf("CRC eeror!\r\n");
//         }


        printf("等等结束\r\n");
        LCD_0IN85_SetBackLight(100);

        Delay_Ms(1000);
        printf("等等结束\r\n");
        LCD_0IN85_SetBackLight(50);
        Delay_Ms(1000);

        //  if (precircle != circle || (precnt != TIM2->CNT && TIM2->CNT % 4 == 0))
        //  {
        //      printf("Encoder position= %d circle %d step\r\n", circle, TIM2->CNT);
        //      if (encodetime != 0)
        //          printf("Encoder speed= %f\r\n", -(float)(precircle * 80 + precnt - (circle * 80 + TIM2->CNT)) / (float)encodetime * 1000.0/(float)SpeedSampleTimeMs / 80.0);
        //      else
        //          printf("Encoder speed null!!\r\n");
        //      encodetime = 0;
        //      precircle = circle;
        //      precnt = TIM2->CNT;
        //  }
        //  encodetime++;
        //  Delay_Ms(SpeedSampleTimeMs);
     }
 }





