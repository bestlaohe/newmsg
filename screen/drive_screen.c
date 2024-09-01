/*
 * drive_screen.c
 *
 *  Created on: 2024年8月30日
 *      Author: 12630
 */
#include "drive_screen.h"


/*********************************************************************
 * @fn      SPI_FullDuplex_Init
 *
 * @brief   Configuring the SPI for full-duplex communication.
 *
 * @return  none
 */
void SPI_FullDuplex_Init(void)
{
    SPI_InitTypeDef  SPI_InitStructure = {0};   // SPI 初始化结构体

    // 使能 GPIOC 和 SPI1 的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    // 配置 SPI 参数
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx; // 设置为全双工模式
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master; // 设置为主模式
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // 数据大小设置为 16 位
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; // 时钟极性设置为高
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; // 时钟相位设置为第二个边缘
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; // NSS 管脚设置为硬件控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; // 波特率预分频设置为 64
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // 数据位顺序设置为 MSB 优先

    SPI_InitStructure.SPI_CRCPolynomial = 10; // CRC 多项式设置为 7
    SPI_Init(SPI1, &SPI_InitStructure); // 初始化 SPI1
    SPI_Cmd(SPI1, ENABLE); // 使能 SPI1


}



int DEV_Module_Init(void)
{
    SPI_FullDuplex_Init();
    LCD_DC_0;
    LCD_CS_0 ;

    TIM_CtrlPWMOutputs(TIM1, ENABLE);

        return 0;
}
//*Slave:SPI1_NSS(PC1)、SPI1_SCK(PC5)、SPI1_MISO(PC7)、SPI1_MOSI(PC6).

void DEV_Module_Exit(void)
{
    LCD_DC_1;
    LCD_CS_1;

    TIM_CtrlPWMOutputs(TIM1, DISABLE);
}
void DEV_SPI_WRite(uint8_t _dat)
{
        SPI_I2S_SendData( SPI1, _dat );

        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) ) ;
}




