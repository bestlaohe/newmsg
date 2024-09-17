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
    SPI_InitTypeDef SPI_InitStructure = {0}; // SPI ��ʼ���ṹ��

    // ʹ�� GPIOC �� SPI1 ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    // ���� SPI ����
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // ����Ϊȫ˫��ģʽ
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                      // ����Ϊ��ģʽ
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  // ���ݴ�С����Ϊ 16 λ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                         // ʱ�Ӽ�������Ϊ��**************************һ��ʼ���ø�Ŷ����Ϊlora�����޸�
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                       // ʱ����λ����Ϊ�ڶ�����Ե*************һ��ʼ����λ�ڶ�����Ϊlora�����޸�
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                          // NSS �ܽ�����ΪӲ������
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // ������Ԥ��Ƶ����Ϊ 64
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                 // ����λ˳������Ϊ MSB ����

    SPI_InitStructure.SPI_CRCPolynomial = 7; // CRC ����ʽ����Ϊ 7
    SPI_Init(SPI1, &SPI_InitStructure);      // ��ʼ�� SPI1
    SPI_Cmd(SPI1, ENABLE);                   // ʹ�� SPI1
}

void SPI_DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize, uint32_t mode)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = mode;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);

    // ʹ��DMA��������ж�
    DMA_ITConfig(DMA_CHx, DMA_IT_TC, ENABLE);

    // ����NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
   // printf("CFGR%x\r\n", DMA1_Channel3->CFGR);
}
void DMA1_Channel3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

int dmacircular = 0;
void DMA1_Channel3_IRQHandler(void)
{

    if (DMA_GetITStatus(DMA1_IT_TC3))
    {
        // ������ɴ���
      //  printf("һ��ʼDMA�������%d\r\n", dmacircular);
        // ����жϱ�־
        DMA_ClearITPendingBit(DMA1_IT_TC3);

        // printf("CFGR%x\r\n", DMA1_Channel3->CFGR);

      //  printf("DMA_Mode_Circular%x\r\n", DMA_Mode_Circular);

       // printf("���%x\r\n", DMA1_Channel3->CFGR & 0x00b0);

        if ((DMA1_Channel3->CFGR & 0x00b0) == 0x00b0) // 0x00b0��ѭ����92������
            dmacircular++;

        if ((DMA1_Channel3->CFGR & 0x00b0) == 0x00b0&& dmacircular == 29)
        {
            dmacircular = 0;
            DMA_Cmd(DMA1_Channel3, DISABLE);
            SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
            DMA_ClearITPendingBit(DMA1_IT_TC3);

            LCD_CS_1;
        }
        Delay_Ms(1);
       // printf("����DMA�������%d\r\n", dmacircular);
    }
}
/*
    ���׵�us������ӳٺ���
    �ҵ�оƬʱ����48Mhz
    һ��ʱ��������1/48M=0.020833us,һ���������ڰ���12��ʱ�����ڣ�����һ������������12*0.020833=0.25us
    ��������i++����һ����0.25us��1us���������4��
*/
void delay_us(uint16_t num)
{
    for (uint16_t j = 0; j < num; j++)
    {
        for (uint16_t i = 0; i < 2; i++)
        {
        } // ���ݲ�ͬ��оƬ�����޸�i����ֵ
    }
}

void DEV_SPI_WRite(uint8_t _dat)
{
    SPI_I2S_SendData(SPI1, _dat);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY))
        ;
}

int LCD_Init(void)
{
    SPI_FullDuplex_Init();
    LCD_DC_0;
    LCD_CS_0;
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    // ����SPI��DMA��������
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);

    // ����DMAͨ��
    DMA_Cmd(DMA1_Channel3, ENABLE);

    // ����DMA�ж�
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);

    return 0;
}

//*Slave:SPI1_NSS(PC1)��SPI1_SCK(PC5)��SPI1_MISO(PC7)��SPI1_MOSI(PC6).
void DEV_Module_Exit(void)
{
    LCD_DC_1;
    LCD_CS_1;

    TIM_CtrlPWMOutputs(TIM1, DISABLE);
}
