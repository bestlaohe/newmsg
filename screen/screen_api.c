/*
 * screen_api.c
 *
 *  Created on: 2024年8月30日
 *      Author: 12630
 */

#include "screen_api.h"
#include <stdlib.h>
#include <stdio.h>
void DrawIcon(int x, int y, int size, int final_size)
{
    Paint_DrawChar(x, y, 0, &Font24_logo, BLACK, WHITE - size * 819, 0);
}

void startup_animation()
{
    int final_size = 48; // 最终图标大小
    int step = 2;        // 每次放大步长
    for (int size = 0; size <= 80; size += step)
    {
        // 计算图标左上角位置，使其中心保持在屏幕中央
        int offset = size / 2;
        DrawIcon(LCD_WIDTH - final_size - offset, LCD_HEIGHT - final_size - offset, size, final_size);
        DrawIcon(offset, offset, size, final_size);
        DrawIcon(LCD_WIDTH - final_size - offset, offset, size, final_size);
        DrawIcon(offset, LCD_HEIGHT - final_size - offset, size, final_size);
    }
    LCD_0IN85_Clear(BLACK);
    Paint_DrawChar(40, 40, 0, &Font24_logo, BLACK, WHITE, 0);
    Delay_Ms(500);
    LCD_0IN85_Clear(BLACK);
}

LCD_0IN85_ATTRIBUTES LCD;
uint8_t lcd_gram[Y_MAX_PIXEL * X_MAX_PIXEL * 2] = {0}; ///< 开辟一块内存空间当显存使用
u8 dmaXoffset, dmaYoffset = 0;
void LCD_SHOW_API_INIT()
{

    LCD_0IN85_Init(VERTICAL);
    Delay_Ms(300);
    // DEBUG_PRINT("Set Clear and Display Funtion\r\n");
    LCD_0IN85_Clear(BLACK);
    //  DEBUG_PRINT("Set Clear and Display Funtion\r\n");
    Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_0, WHITE);

    //  DEBUG_PRINT("Set Clear and Display Funtion\r\n");

    //    LCD_0IN85_Init(VERTICAL);
    //      LCD_0IN85_Clear(BLACK);
    //
    //      DEBUG_PRINT("Paint_NewImage\r\n");
    //      Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_180, WHITE);
    //
    //      DEBUG_PRINT("Set Clear and Display Funtion\r\n");
    //      //Paint_SetClearFuntion(LCD_0IN85_Clear);
    //     // Paint_SetDisplayFuntion(LCD_0IN85_DrawPaint);
    //
    //      DEBUG_PRINT("Paint_Clear\r\n");
    //      //Paint_Clear(BLACK);
    //
    //      DEBUG_PRINT("drawing...\r\n");
    //      Paint_SetRotate(0);

    //  DEBUG_PRINT("Set Clear and Display Funtion\r\n");
    // Paint_SetClearFuntion(LCD_0IN85_Clear);
    // Paint_SetDisplayFuntion(LCD_0IN85_DrawPaint);

    //  DEBUG_PRINT("LCD_0IN85_Clear\r\n");

    // DEBUG_PRINT("drawing...\r\n");
    //  Paint_SetRotate(0);

    //       Paint_DrawString(5, 10, "123",        &Font24,  YELLOW, RED);
    //       Paint_DrawString(5, 34, "ABC",        &Font24,  BLUE,   CYAN);

    //  Paint_DrawFloatNum (5, 58 ,987.654321,3, &Font12,  WHITE,  BLACK);
    //  Paint_DrawString_CN(0,80, "微雪电子",   &Font24CN,WHITE,  RED);
    //     Paint_DrawImage(set,30,3,48,48);
    //    Delay_Ms(3000);
    //    LCD_0IN85_Clear(WHITE);
    //
    ////  Paint_DrawRectangle(0, 10, 225, 58, RED     ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
    ////  Paint_DrawLine  (0, 10, 225, 58,    MAGENTA ,DOT_PIXEL_2X2,LINE_STYLE_SOLID);
    ////  Paint_DrawLine  (0, 10, 125, 58,    MAGENTA ,DOT_PIXEL_2X2,LINE_STYLE_SOLID);
    //    Paint_DrawCircle(35,50,  25,        BLUE    ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
    //    Paint_DrawCircle(65,50,  25,        BLACK   ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
    //    Paint_DrawCircle(95,50,  25,        RED     ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
    //    Paint_DrawCircle(50,75,  25,        YELLOW  ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
    //    Paint_DrawCircle(80,75,  25,        GREEN   ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
    //    Delay_Ms(3000);
    //
    //    DEBUG_PRINT("quit...\r\n");
    //    //DEV_Module_Exit();
}

/*****************************************************************************
 * | File        :   LCD_0IN85.c
 * | Author      :   Waveshare team
 * | Function    :   Hardware underlying interface
 * | Info        :
 *                Used to shield the underlying layers of each master
 *                and enhance portability
 *----------------
 * | This version:   V1.0
 * | Date        :   2018-01-11
 * | Info        :   Basic version
 *
 ******************************************************************************/

// LCD_0IN85_ATTRIBUTES LCD;

/******************************************************************************
function :  send command
parameter:
     Reg : Command register
******************************************************************************/
static void LCD_0IN85_SendCommand(UBYTE Reg)
{

    LCD_DC_0;
    LCD_CS_ENABLE;
    DEV_SPI_WRite(Reg);
    LCD_CS_DISABLE;
}

/******************************************************************************
function :  send data
parameter:
    Data : Write data
******************************************************************************/
static void LCD_0IN85_SendData_8Bit(UBYTE Data)
{

    LCD_DC_1;
    LCD_CS_ENABLE;
    DEV_SPI_WRite(Data);
    LCD_CS_DISABLE;
}

/******************************************************************************
function :  send data
parameter:
    Data : Write data
******************************************************************************/
#if !USE_DMA
static void LCD_0IN85_SendData_16Bit(UWORD Data)
{
    LCD_DC_1;
    LCD_CS_ENABLE;
    DEV_SPI_WRite((Data >> 8) & 0xFF);
    DEV_SPI_WRite(Data & 0xFF);
    LCD_CS_DISABLE;
}
#endif

/******************************************************************************
function :  Initialize the lcd register
parameter:
******************************************************************************/
// 常量数组存储命令和数据
static const uint8_t init_cmds[] = {
    0xB0, 0xC0,
    0xB2, 0x2F,
    0xB3, 0x03,
    0xB6, 0x19,
    0xB7, 0x01,
    0xAC, 0xCB,
    0xAB, 0x0E,
    0xB4, 0x04,
    0xA8, 0x19,
    0x3A, 0x05,
    0xB8, 0x08,
    0xE8, 0x24,
    0xE9, 0x48,
    0xEA, 0x22,
    0xC6, 0x30,
    0xC7, 0x18,
    0xF0, 0x1F, 0x28, 0x04, 0x3E, 0x2A, 0x2E, 0x20, 0x00, 0x0C, 0x06, 0x00, 0x1C, 0x1F, 0x0F,
    0xF1, 0x00, 0x2D, 0x2F, 0x3C, 0x6F, 0x1C, 0x0B, 0x00, 0x00, 0x00, 0x07, 0x0D, 0x11, 0x0F,
    0x21,
    0x11,
    0x29};

static void LCD_0IN85_InitReg(void)
{
    for (int8_t i = 0; i < sizeof(init_cmds); ++i)
    {
        if (i % 2 == 0)
        {
            LCD_0IN85_SendCommand(init_cmds[i]);
        }
        else
        {
            LCD_0IN85_SendData_8Bit(init_cmds[i]);
        }
    }
}

/********************************************************************************
function:   Set the resolution and scanning method of the screen
parameter:
        Scan_dir:   Scan direction
********************************************************************************/
static void LCD_0IN85_SetAttributes(UBYTE Scan_dir)
{
    // Get the screen scan direction
    LCD.SCAN_DIR = Scan_dir;
    UBYTE MemoryAccessReg = 0x00;

    // Get GRAM and LCD width and height
    // if (Scan_dir == HORIZONTAL)
    // {
    //     LCD.HEIGHT = LCD_HEIGHT;
    //     LCD.WIDTH = LCD_WIDTH;
    //     MemoryAccessReg = 0X70;
    // }
    // else
    // {
    LCD.HEIGHT = LCD_WIDTH;
    LCD.WIDTH = LCD_HEIGHT;
    MemoryAccessReg = 0XC8;
    // }

    // Set the read / write scan direction of the frame memory
    LCD_0IN85_SendCommand(0x36);              // MX, MY, RGB mode
    LCD_0IN85_SendData_8Bit(MemoryAccessReg); // 0x08 set RGB
}

static void LCD_0IN85_Reset(void)
{
 
    LCD_RST_0;
    Delay_Ms(20);
    LCD_RST_1;
    Delay_Ms(20);
}

/********************************************************************************
function :  Initialize the lcd
parameter:
********************************************************************************/
void LCD_0IN85_Init(UBYTE Scan_dir)
{

    // Hardware reset
    LCD_0IN85_Reset();

    // Set the resolution and scanning method of the screen
    LCD_0IN85_SetAttributes(Scan_dir);

    // Set the initialization register
    LCD_0IN85_InitReg();
}

/********************************************************************************
function:   Sets the start position and size of the display area
parameter:
        Xstart  :   X direction Start coordinates
        Ystart  :   Y direction Start coordinates
        Xend    :   X direction end coordinates
        Yend    :   Y direction end coordinates
********************************************************************************/
void LCD_0IN85_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    dmaXoffset = 0;
    dmaYoffset = 0;

    Ystart = Ystart + 1;
    Yend = Yend + 1;

    Xstart = Xstart + 2;
    Xend = Xend + 2;

    // set the X coordinates
    LCD_0IN85_SendCommand(0x2A);
    LCD_0IN85_SendData_8Bit((Xstart >> 8) & 0xFF);
    LCD_0IN85_SendData_8Bit(Xstart & 0xFF);
    LCD_0IN85_SendData_8Bit(((Xend) >> 8) & 0xFF);
    LCD_0IN85_SendData_8Bit((Xend) & 0xFF);

    // set the Y coordinates
    LCD_0IN85_SendCommand(0x2B);
    LCD_0IN85_SendData_8Bit((Ystart >> 8) & 0xFF);
    LCD_0IN85_SendData_8Bit(Ystart & 0xFF);
    LCD_0IN85_SendData_8Bit(((Yend) >> 8) & 0xFF);
    LCD_0IN85_SendData_8Bit((Yend) & 0xFF);

    LCD_0IN85_SendCommand(0X2C);
}

/******************************************************************************
function :  Clear screen
parameter:
******************************************************************************/

void LCD_0IN85_Clear(UWORD Color)
{

    // 简单的清屏，需要测试

#if USE_DMA
    while (!READ_LORA_CS)
    {
        DEBUG_PRINT("wait lora cs 1!\r\n");
    }
    LCD_0IN85_SetWindows(0, 0, LCD_WIDTH, LCD_HEIGHT);
    static u16 save_color;
    save_color = Color;

    LCD_DC_1;
    LCD_CS_ENABLE;

    // 初始化DMA传输
    SPI_DMA_Tx_Init(DMA1_Channel3, (u32)&SPI1->DATAR, (u32)save_color, Y_MAX_PIXEL * X_MAX_PIXEL * 2, DMA_Mode_Normal, DMA_MemoryInc_Disable);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA1_Channel3, ENABLE);

    while (!dmaTransferComplete)
    {
        //  DEBUG_PRINT("wait normoldma ok %d\r\n", dmaTransferComplete); // 等待通道3传输完成标志
    }
    dmaTransferComplete = 0;
    DMA_ClearFlag(DMA1_FLAG_TC3); // 清除通道3传输完成标志

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        ;
   
    DMA_Cmd(DMA1_Channel3, DISABLE);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
    DMA_ClearITPendingBit(DMA1_IT_TC3);
     LCD_CS_DISABLE;
#endif

#if 0
    while (!READ_LORA_CS)
    {
        DEBUG_PRINT("wait lora cs 1!\r\n");
    }
    int dma_circular = 0;
    LCD_0IN85_SetWindows(0, 0, LCD_WIDTH, LCD_HEIGHT);
    int index = 0; // 用于跟踪lcd_gram数组的索引

    // 用于减少临时变量占用的内存
    uint8_t high_byte = (Color >> 8) & 0xff; // 高字节
    uint8_t low_byte = Color & 0xff;         // 低字节

    for (int i = 0; i < X_MAX_PIXEL; i++)
    {
        for (int j = 0; j < Y_MAX_PIXEL; j++)
        {
            lcd_gram[index++] = high_byte; // 高字节
            lcd_gram[index++] = low_byte;  // 低字节
        }
    }

    LCD_DC_1;
    LCD_CS_ENABLE;

    // 初始化DMA传输
    SPI_DMA_Tx_Init(DMA1_Channel3, (u32)&SPI1->DATAR, (u32)lcd_gram, Y_MAX_PIXEL * X_MAX_PIXEL * 2, DMA_Mode_Circular,DMA_MemoryInc_Enable);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA1_Channel3, ENABLE);

    // 计算总传输次数
    int total_dma_transfers = (LCD_HEIGHT * LCD_WIDTH / (X_MAX_PIXEL * Y_MAX_PIXEL)) + 1;

    while (dma_circular <= total_dma_transfers)
    {
        if (dmaTransferComplete)
        {
            dmaTransferComplete = 0;
            dma_circular++;

            // 打印当前传输次数，帮助调试
            DEBUG_PRINT("Current DMA transfers: %d/%d\r\n", dma_circular, total_dma_transfers);
        }
    }

    // 重置计数器
    dma_circular = 0;

    // 等待SPI发送缓冲区为空
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
        // 可以考虑在此处添加超时逻辑，以防死循环
        DEBUG_PRINT("Waiting for SPI transmit buffer to be empty...\r\n"); // 打印等待信息
    }

    // 禁用DMA和SPI
    DMA_Cmd(DMA1_Channel3, DISABLE);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
    DMA_ClearITPendingBit(DMA1_IT_TC3);

    LCD_CS_DISABLE;
    DEBUG_PRINT("LCD_0IN85_Clear OK\r\n"); // 等待SPI发送缓冲区为空

#else
    UWORD i, j;
    LCD_0IN85_SetWindows(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    LCD_DC_1;
    for (i = 0; i < LCD_WIDTH; i++)
    {
        for (j = 0; j < LCD_HEIGHT; j++)
        {
            LCD_DC_1;
            LCD_CS_ENABLE;
            DEV_SPI_WRite((Color >> 8) & 0xff);
            DEV_SPI_WRite(Color & 0xff);
            LCD_CS_DISABLE;
        }
    }
#endif
}

/******************************************************************************
function :  Sends the image buffer in RAM to displays
parameter:
******************************************************************************/
void LCD_0IN85_Display(UWORD *Image)
{
    UWORD i, j;
    LCD_0IN85_SetWindows(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    LCD_DC_1;
    for (i = 0; i < LCD_WIDTH; i++)
    {
        for (j = 0; j < LCD_HEIGHT; j++)
        {
            DEV_SPI_WRite((*(Image + i * LCD_WIDTH + j) >> 8) & 0xff);
            DEV_SPI_WRite(*(Image + i * LCD_WIDTH + j));
        }
    }
}

void LCD_0IN85_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image)
{
    // display
    UDOUBLE Addr = 0;

    UWORD i, j;
    LCD_0IN85_SetWindows(Xstart, Ystart, Xend - 1, Yend - 1);
    LCD_DC_1;
    for (i = Ystart; i < Yend - 1; i++)
    {
        Addr = Xstart + i * LCD_WIDTH;
        for (j = Xstart; j < Xend - 1; j++)
        {
            DEV_SPI_WRite((*(Image + Addr + j) >> 8) & 0xff);
            DEV_SPI_WRite(*(Image + Addr + j));
        }
    }
}

void Lcd_Refrsh_DMA(int pic_size)
{

#if USE_DMA
    while (!READ_LORA_CS)
    {
        DEBUG_PRINT("wait lora cs 1!\r\n");
    }

    // 将整个数据搬运一次到DMA
    LCD_DC_1;
    LCD_CS_ENABLE;
    // DEBUG_PRINT("开始刷屏\r\n");
    dmaTransferComplete = 0;
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
    DMA_Cmd(DMA1_Channel3, DISABLE);

    SPI_DMA_Tx_Init(DMA1_Channel3, (u32)&SPI1->DATAR, (u32)lcd_gram, pic_size, DMA_Mode_Normal, DMA_MemoryInc_Enable);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA1_Channel3, ENABLE);

    while (!dmaTransferComplete)
    {
        //  DEBUG_PRINT("wait normoldma ok %d\r\n", dmaTransferComplete); // 等待通道3传输完成标志
    }
    dmaTransferComplete = 0;
    DMA_ClearFlag(DMA1_FLAG_TC3); // 清除通道3传输完成标志

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        ;
    Delay_Ms(1); // 发送完成后要等一下彻底完成

    DMA_Cmd(DMA1_Channel3, DISABLE);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
    DMA_ClearITPendingBit(DMA1_IT_TC3);

    LCD_CS_DISABLE;

#endif
}
/******************************************************************************
function: Draw a point
parameter   :
        X   :   Set the X coordinate
        Y   :   Set the Y coordinate
      Color :   Set the color
******************************************************************************/
void LCD_0IN85_DrawPaint(UWORD x, UWORD y, UWORD Color)
{

    //
    //   DEBUG_PRINT("x = %d, y = %d,dmaXpoint:%d,dmaYpoint:%d\r\n",x,y,dmaXpoint,dmaYpoint);
    //      DEBUG_PRINT("dmaFont->Width:%d\r\n", dmaFont->Width);
    //

#if USE_DMA

    ///< 使用DMA的话，从对点刷屏到对显存数组写入数据，DMA传输数据的时候再统一进行传输
    int index = ((y - dmaYpoint) * (dmaFont->Width) + (x - dmaXpoint)) * 2;
    // DEBUG_PRINT("index开始偏移前:%d\r\n", index );

    index = index - X_MAX_PIXEL * Y_MAX_PIXEL * 2 * dmaXoffset / X_MAX_PIXEL;
    lcd_gram[index] = (Color >> 8) & 0xff; // 高字节
    lcd_gram[index + 1] = Color & 0xFF;    // 低字节
                                           //  DEBUG_PRINT("开始偏移:%d\r\n", index + 1);
    if ((index + 1) == (Y_MAX_PIXEL * X_MAX_PIXEL * 2 - 1))
    {

        Lcd_Refrsh_DMA(Y_MAX_PIXEL * X_MAX_PIXEL * 2);
        dmaXoffset = X_MAX_PIXEL + dmaXoffset;
        dmaYoffset = Y_MAX_PIXEL + dmaYoffset;
        DEBUG_PRINT("dma ok\r\n");
    }

#else
    LCD_0IN85_SetWindows(x, y, x, y);
    LCD_0IN85_SendData_16Bit(Color);
#endif
}

/*******************************************************************************
function:
    Setting backlight
parameter   :
      value : Range 0~1000   Duty cycle is value/1000
*******************************************************************************/
void LCD_0IN85_SetBackLight(UWORD Value)
{
    DEV_Set_PWM(Value);
}
