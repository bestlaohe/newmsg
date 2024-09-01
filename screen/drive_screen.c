/*
 * drive_screen.c
 *
 *  Created on: 2024��8��30��
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
    SPI_InitTypeDef  SPI_InitStructure = {0};   // SPI ��ʼ���ṹ��

    // ʹ�� GPIOC �� SPI1 ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    // ���� SPI ����
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx; // ����Ϊȫ˫��ģʽ
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master; // ����Ϊ��ģʽ
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // ���ݴ�С����Ϊ 16 λ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; // ʱ�Ӽ�������Ϊ��
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; // ʱ����λ����Ϊ�ڶ�����Ե
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; // NSS �ܽ�����ΪӲ������
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; // ������Ԥ��Ƶ����Ϊ 64
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // ����λ˳������Ϊ MSB ����

    SPI_InitStructure.SPI_CRCPolynomial = 10; // CRC ����ʽ����Ϊ 7
    SPI_Init(SPI1, &SPI_InitStructure); // ��ʼ�� SPI1
    SPI_Cmd(SPI1, ENABLE); // ʹ�� SPI1


}



int DEV_Module_Init(void)
{
    SPI_FullDuplex_Init();
    LCD_DC_0;
    LCD_CS_0 ;

    TIM_CtrlPWMOutputs(TIM1, ENABLE);

        return 0;
}
//*Slave:SPI1_NSS(PC1)��SPI1_SCK(PC5)��SPI1_MISO(PC7)��SPI1_MOSI(PC6).

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




