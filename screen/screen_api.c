/*
 * screen_api.c
 *
 *  Created on: 2024年8月30日
 *      Author: 12630
 */

#include "screen_api.h"
#include <stdlib.h>
#include <stdio.h>
void startup_animation()
{

    int final_size = 48; // 最终图标大小
    int step = 2;        // 每次放大步长

    for (int size = 0; size <= 80; size += step)
    {

        // 计算图标左上角位置，使其中心保持在屏幕中央
        int icon_x = LCD_WIDTH - final_size - size / 2;
        int icon_y = LCD_HEIGHT - final_size - size / 2;

        Paint_Drawicon(icon_x, icon_y, 0, &Font24_logo, BLACK, WHITE - size * 819);

        icon_x = size / 2;
        icon_y = size / 2;
        Paint_Drawicon(icon_x, icon_y, 0, &Font24_logo, BLACK, WHITE - size * 819);

        // 从右上角移动到中心
        icon_x = LCD_WIDTH - final_size - size / 2;
        icon_y = size / 2;
        Paint_Drawicon(icon_x, icon_y, 0, &Font24_logo, BLACK, WHITE - size * 819);

        // 从左下角移动到中心
        icon_x = size / 2;
        icon_y = LCD_HEIGHT - final_size - size / 2;
        Paint_Drawicon(icon_x, icon_y, 0, &Font24_logo, BLACK, WHITE - size * 819);
    }
    Paint_Clear(BLACK);
    Paint_Drawicon(40, 40, 0, &Font24_logo, BLACK, WHITE);
    Delay_Ms(500);
    Paint_Clear(BLACK);
}

LCD_0IN85_ATTRIBUTES LCD;
uint8_t lcd_gram[Y_MAX_PIXEL * X_MAX_PIXEL * 2] = {0}; ///< 开辟一块内存空间当显存使用
u8 dmaXoffset, dmaYoffset = 0;
void LCD_SHOW_API_INIT()
{

    LCD_0IN85_Init(VERTICAL);
    LCD_0IN85_Clear(BLACK);

    printf("Paint_NewImage\r\n");
    Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_180, WHITE);

    printf("Set Clear and Display Funtion\r\n");
    Paint_SetClearFuntion(LCD_0IN85_Clear);
    Paint_SetDisplayFuntion(LCD_0IN85_DrawPaint);

    printf("Paint_Clear\r\n");
    Paint_Clear(BLACK);

    printf("drawing...\r\n");
    Paint_SetRotate(0);

    //  Paint_Drawicon(40, 40, 1,&Font24_icon,  BLACK, BLUE);

    //       Paint_DrawString(5, 10, "123",        &Font24,  YELLOW, RED);
    //       Paint_DrawString(5, 34, "ABC",        &Font24,  BLUE,   CYAN);

    //  Paint_DrawFloatNum (5, 58 ,987.654321,3, &Font12,  WHITE,  BLACK);
    //  Paint_DrawString_CN(0,80, "微雪电子",   &Font24CN,WHITE,  RED);
    //     Paint_DrawImage(set,30,3,48,48);
    //    Delay_Ms(3000);
    //    Paint_Clear(WHITE);
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
    //    printf("quit...\r\n");
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
    LCD_CS_0;
    DEV_SPI_WRite(Reg);
    LCD_CS_1;
}

/******************************************************************************
function :  send data
parameter:
    Data : Write data
******************************************************************************/
static void LCD_0IN85_SendData_8Bit(UBYTE Data)
{
    LCD_DC_1;
    LCD_CS_0;
    DEV_SPI_WRite(Data);
    LCD_CS_1;
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
    LCD_CS_0;
    DEV_SPI_WRite((Data >> 8) & 0xFF);
    DEV_SPI_WRite(Data & 0xFF);
    LCD_CS_1;
}
#endif

/******************************************************************************
function :  Initialize the lcd register
parameter:
******************************************************************************/
static void LCD_0IN85_InitReg(void)
{

    LCD_0IN85_SendCommand(0xB0);
    LCD_0IN85_SendData_8Bit(0xC0);
    LCD_0IN85_SendCommand(0xB2);
    LCD_0IN85_SendData_8Bit(0x2F);
    LCD_0IN85_SendCommand(0xB3);
    LCD_0IN85_SendData_8Bit(0x03);
    LCD_0IN85_SendCommand(0xB6);
    LCD_0IN85_SendData_8Bit(0x19);
    LCD_0IN85_SendCommand(0xB7);
    LCD_0IN85_SendData_8Bit(0x01);

    LCD_0IN85_SendCommand(0xAC);
    LCD_0IN85_SendData_8Bit(0xCB);
    LCD_0IN85_SendCommand(0xAB);
    LCD_0IN85_SendData_8Bit(0x0e);

    LCD_0IN85_SendCommand(0xB4);
    LCD_0IN85_SendData_8Bit(0x04);

    LCD_0IN85_SendCommand(0xA8);
    LCD_0IN85_SendData_8Bit(0x19);

    LCD_0IN85_SendCommand(0x3A);
    LCD_0IN85_SendData_8Bit(0x05);

    LCD_0IN85_SendCommand(0xb8);
    LCD_0IN85_SendData_8Bit(0x08);

    LCD_0IN85_SendCommand(0xE8);
    LCD_0IN85_SendData_8Bit(0x24);

    LCD_0IN85_SendCommand(0xE9);
    LCD_0IN85_SendData_8Bit(0x48);

    LCD_0IN85_SendCommand(0xea);
    LCD_0IN85_SendData_8Bit(0x22);

    LCD_0IN85_SendCommand(0xC6);
    LCD_0IN85_SendData_8Bit(0x30);
    LCD_0IN85_SendCommand(0xC7);
    LCD_0IN85_SendData_8Bit(0x18);

    LCD_0IN85_SendCommand(0xF0);
    LCD_0IN85_SendData_8Bit(0x1F);
    LCD_0IN85_SendData_8Bit(0x28);
    LCD_0IN85_SendData_8Bit(0x04);
    LCD_0IN85_SendData_8Bit(0x3E);
    LCD_0IN85_SendData_8Bit(0x2A);
    LCD_0IN85_SendData_8Bit(0x2E);
    LCD_0IN85_SendData_8Bit(0x20);
    LCD_0IN85_SendData_8Bit(0x00);
    LCD_0IN85_SendData_8Bit(0x0C);
    LCD_0IN85_SendData_8Bit(0x06);
    LCD_0IN85_SendData_8Bit(0x00);
    LCD_0IN85_SendData_8Bit(0x1C);
    LCD_0IN85_SendData_8Bit(0x1F);
    LCD_0IN85_SendData_8Bit(0x0f);

    LCD_0IN85_SendCommand(0xF1);
    LCD_0IN85_SendData_8Bit(0X00);
    LCD_0IN85_SendData_8Bit(0X2D);
    LCD_0IN85_SendData_8Bit(0X2F);
    LCD_0IN85_SendData_8Bit(0X3C);
    LCD_0IN85_SendData_8Bit(0X6F);
    LCD_0IN85_SendData_8Bit(0X1C);
    LCD_0IN85_SendData_8Bit(0X0B);
    LCD_0IN85_SendData_8Bit(0X00);
    LCD_0IN85_SendData_8Bit(0X00);
    LCD_0IN85_SendData_8Bit(0X00);
    LCD_0IN85_SendData_8Bit(0X07);
    LCD_0IN85_SendData_8Bit(0X0D);
    LCD_0IN85_SendData_8Bit(0X11);
    LCD_0IN85_SendData_8Bit(0X0f);

    LCD_0IN85_SendCommand(0x21);

    LCD_0IN85_SendCommand(0x11);
    LCD_0IN85_SendCommand(0x29);
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
    if (Scan_dir == HORIZONTAL)
    {
        LCD.HEIGHT = LCD_HEIGHT;
        LCD.WIDTH = LCD_WIDTH;
        MemoryAccessReg = 0X70;
    }
    else
    {
        LCD.HEIGHT = LCD_WIDTH;
        LCD.WIDTH = LCD_HEIGHT;
        MemoryAccessReg = 0XC8;
    }

    // Set the read / write scan direction of the frame memory
    LCD_0IN85_SendCommand(0x36);              // MX, MY, RGB mode
    LCD_0IN85_SendData_8Bit(MemoryAccessReg); // 0x08 set RGB
}

static void LCD_0IN85_Reset(void)
{
    LCD_CS_0;
    Delay_Ms(20);
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
    UWORD i, j;

#if USE_DMA

    LCD_0IN85_SetWindows(0, 0, LCD_WIDTH, LCD_HEIGHT);
    int index = 0; // 用于跟踪lcd_gram数组的索引
    for (i = 0; i < X_MAX_PIXEL; i++)
    {
        for (j = 0; j < Y_MAX_PIXEL; j++)
        {
            lcd_gram[index++] = (Color >> 8) & 0xff; // 高字节
            lcd_gram[index++] = Color & 0xff;        // 低字节
        }
    }

    LCD_DC_1;
    LCD_CS_0;

    SPI_DMA_Tx_Init(DMA1_Channel3, (u32)&SPI1->DATAR, (u32)lcd_gram, Y_MAX_PIXEL * X_MAX_PIXEL * 2, DMA_Mode_Circular);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA1_Channel3, ENABLE);

    // while (DMA_GetFlagStatus(DMA1_FLAG_TC3) == RESET)
    //     printf("等待通道3传输完成标志\r\n"); // 等待通道3传输完成标志
    DMA_ClearFlag(DMA1_FLAG_TC3); // 清除通道3传输完成标志
    // while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    //     printf("等待SPI发送缓冲区为空\r\n"); // 等待SPI发送缓冲区为空
    dmacircular = 0;

#else

    LCD_0IN85_SetWindows(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    LCD_DC_1;
    for (i = 0; i < LCD_WIDTH; i++)
    {
        for (j = 0; j < LCD_HEIGHT; j++)
        {
            LCD_CS_0;
            LCD_DC_1;
            DEV_SPI_WRite((Color >> 8) & 0xff);
            DEV_SPI_WRite(Color & 0xff);
            LCD_CS_1;
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
    // 将整个数据搬运一次到DMA
    LCD_DC_1;
    LCD_CS_0;
    // printf("开始刷屏\r\n");
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
    DMA_Cmd(DMA1_Channel3, DISABLE);

    // if (dmaFont->Width * dmaFont->Height * 2 < Y_MAX_PIXEL * X_MAX_PIXEL * 2)
    //    SPI_DMA_Tx_Init(DMA1_Channel3, (u32)&SPI1->DATAR, (u32)lcd_gram, dmaFont->Width * dmaFont->Height * 2, DMA_Mode_Normal);
    // else
    //     SPI_DMA_Tx_Init(DMA1_Channel3, (u32)&SPI1->DATAR, (u32)lcd_gram, Y_MAX_PIXEL * X_MAX_PIXEL * 2, DMA_Mode_Normal);

    SPI_DMA_Tx_Init(DMA1_Channel3, (u32)&SPI1->DATAR, (u32)lcd_gram, pic_size, DMA_Mode_Normal);

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA1_Channel3, ENABLE);

    // while (DMA_GetFlagStatus(DMA1_FLAG_TC3) == RESET)
    //     printf("等待通道3传输完成标志\r\n"); // 等待通道3传输完成标志

    // DMA_ClearFlag(DMA1_FLAG_TC3);            // 清除通道3传输完成标志

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        ;
    Delay_Ms(1); // 发送完成后要等一下彻底完成
    // printf("等待SPI发送缓冲区为空\r\n"); // 等待SPI发送缓冲区为空
    LCD_CS_1;

    while (dmacircular != 0)
        printf("等待循环dma：%d\r\n", dmacircular);

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
//   printf("x = %d, y = %d,dmaXpoint:%d,dmaYpoint:%d\r\n",x,y,dmaXpoint,dmaYpoint);
//      printf("dmaFont->Width:%d\r\n", dmaFont->Width);
//


#if USE_DMA
 

    ///< 使用DMA的话，从对点刷屏到对显存数组写入数据，DMA传输数据的时候再统一进行传输
    int index = ((y - dmaYpoint) * (dmaFont->Width) + (x - dmaXpoint)) * 2;
    //printf("index开始偏移前:%d\r\n", index );

    index = index - X_MAX_PIXEL * Y_MAX_PIXEL * 2 * dmaXoffset / X_MAX_PIXEL;
    lcd_gram[index] = (Color >> 8) & 0xff; // 高字节
    lcd_gram[index + 1] = Color & 0xFF;    // 低字节
                                          //  printf("开始偏移:%d\r\n", index + 1);
    if ((index + 1) == (Y_MAX_PIXEL * X_MAX_PIXEL * 2 - 1))
    {

        Lcd_Refrsh_DMA(Y_MAX_PIXEL * X_MAX_PIXEL * 2);
        dmaXoffset = X_MAX_PIXEL + dmaXoffset;
        dmaYoffset = Y_MAX_PIXEL + dmaYoffset;
         printf("dma ok\r\n");
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
