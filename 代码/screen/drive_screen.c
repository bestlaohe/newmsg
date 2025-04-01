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
    SPI_InitTypeDef SPI_InitStructure = {0}; // SPI 初始化结构体

    // 使能 GPIOC 和 SPI1 的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    // 配置 SPI 参数
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 设置为全双工模式
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                      // 设置为主模式
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  // 数据大小设置为 16 位
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                         // 时钟极性设置为高**************************一开始设置高哦，因为lora所以修改
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                       // 时钟相位设置为第二个边缘*************一开始设置位第二，因为lora所以修改
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                          // NSS 管脚设置为硬件控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // 波特率预分频设置为 64
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                 // 数据位顺序设置为 MSB 优先

    SPI_InitStructure.SPI_CRCPolynomial = 7; // CRC 多项式设置为 7
    SPI_Init(SPI1, &SPI_InitStructure);      // 初始化 SPI1
    SPI_Cmd(SPI1, ENABLE);                   // 使能 SPI1
}

void SPI_DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize, uint32_t mode, uint32_t MemoryInc)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = MemoryInc;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = mode;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);

    // 使能DMA传输完成中断
    DMA_ITConfig(DMA_CHx, DMA_IT_TC, ENABLE);

    // 配置NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
    简易的us级别的延迟函数
    我的芯片时钟是48Mhz
    一个时钟周期是1/48M=0.020833us,一个机器周期包含12个时钟周期，所以一个机器周期是12*0.020833=0.25us
    所以这里i++运行一次是0.25us，1us大概是运行4次
*/
void delay_us(uint16_t num)
{
    for (uint16_t j = 0; j < num; j++)
    {
        for (uint16_t i = 0; i < 2; i++)
        {
        } // 根据不同的芯片周期修改i的数值
    }
}

u8 Screen_spi_write(uint8_t _dat)
{
    while (!READ_LORA_CS)
    {
        DEBUG_PRINT("wait lora cs 1!\r\n");
    }
    u8 retry = 0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
        retry++;
        if (retry > 200)
        {
            DEBUG_PRINT("send spi outtime!\r\n");
            return 0;
        }

    } // 还没发完
    SPI_I2S_SendData(SPI1, (uint16_t)_dat);

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    {
        retry++;
        if (retry > 200)
        {
            DEBUG_PRINT("rec spi outtime!\r\n");
            return 0;
        }
    } // 还没收完

    return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}




