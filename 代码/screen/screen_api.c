/*
 * Screen_api.c
 *
 *  Created on: 2024年8月30日
 *      Author: 12630
 */

#include "Screen_api.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
void startup_animation()
{
    for (int s = 0, o; s <= 80; s += 2)
    {
        o = s / 2;
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                Paint_DrawChar(
                    i * (LCD_WIDTH - 48 - o) + (1 - i) * o,
                    j * (LCD_HEIGHT - 48 - o) + (1 - j) * o,
                    0, &Font24_logo, BLACK, WHITE - s * 819, 0);
    }
    Screen_Clear(0, 0, 127, 127, BLACK);
    Paint_DrawChar(40, 40, 0, &Font24_logo, BLACK, WHITE, 0);
    Delay_Ms(500);
    Screen_Clear(0, 0, 127, 127, BLACK);
}

Screen_ATTRIBUTES LCD;
uint8_t lcd_gram[Y_MAX_PIXEL * X_MAX_PIXEL * 2] = {0}; ///< 开辟一块内存空间当显存使用
u8 dmaXoffset, dmaYoffset = 0;
void LCD_SHOW_API_INIT()
{

    Screen_Init(VERTICAL);
    //    Delay_Ms(300);
    // DEBUG_PRINT("Set Clear and Display Funtion\r\n");
     Screen_Clear(0, 0, 128, 128, MY_THEME_BACK_COLOR);
     
    //  DEBUG_PRINT("Set Clear and Display Funtion\r\n");
    Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_0, WHITE);


   
    //  DEBUG_PRINT("Set Clear and Display Funtion\r\n");

    //    Screen_Init(VERTICAL);
    //      Screen_Clear(BLACK);
    //
    //      DEBUG_PRINT("Paint_NewImage\r\n");
    //      Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_180, WHITE);
    //
    //      DEBUG_PRINT("Set Clear and Display Funtion\r\n");
    //      //Paint_SetClearFuntion(Screen_Clear);
    //     // Paint_SetDisplayFuntion(Screen_DrawPaint);
    //
    //      DEBUG_PRINT("Paint_Clear\r\n");
    //      //Paint_Clear(BLACK);
    //
    //      DEBUG_PRINT("drawing...\r\n");
    //      Paint_SetRotate(0);

    //  DEBUG_PRINT("Set Clear and Display Funtion\r\n");
    // Paint_SetClearFuntion(Screen_Clear);
    // Paint_SetDisplayFuntion(Screen_DrawPaint);

    //  DEBUG_PRINT("Screen_Clear\r\n");

    // DEBUG_PRINT("drawing...\r\n");
    //  Paint_SetRotate(0);

    //       Paint_DrawString(5, 10, "123",        &Font24,  YELLOW, RED);
    //       Paint_DrawString(5, 34, "ABC",        &Font24,  BLUE,   CYAN);

    //  Paint_DrawFloatNum (5, 58 ,987.654321,3, &Font12,  WHITE,  BLACK);
    //  Paint_DrawString_CN(0,80, "微雪电子",   &Font24CN,WHITE,  RED);
    //     Paint_DrawImage(set,30,3,48,48);
    //    Delay_Ms(3000);
    //    Screen_Clear(WHITE);
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
}

/*****************************************************************************
 * | File        :   screen.c
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

// Screen_ATTRIBUTES LCD;

/******************************************************************************
function :  send command
parameter:
     Reg : Command register
******************************************************************************/
void Screen_SendCommand(UBYTE Reg)
{
    LCD_DC_0;
    LCD_CS_ENABLE;
    Screen_spi_write(Reg);
    LCD_CS_DISABLE;
}

/******************************************************************************
function :  send data
parameter:
    Data : Write data
******************************************************************************/
static void Screen_SendData_8Bit(UBYTE Data)
{

    LCD_DC_1;
    LCD_CS_ENABLE;
    Screen_spi_write(Data);
    LCD_CS_DISABLE;
}

/******************************************************************************
function :  send data
parameter:
    Data : Write data
******************************************************************************/
#if !USE_DMA
static void Screen_SendData_16Bit(UWORD Data)
{
    LCD_DC_1;
    LCD_CS_ENABLE;
    Screen_spi_write((Data >> 8) & 0xFF);
    Screen_spi_write(Data & 0xFF);
    LCD_CS_DISABLE;
}
#endif
int LCD_Drive_Init(void)
{
    SPI_FullDuplex_Init();
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

#if USE_DMA
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE); // 启用SPI的DMA发送请求
    DMA_Cmd(DMA1_Channel3, ENABLE);                  // 启用DMA通道
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);              // 启用DMA中断
#endif
    Screen_ExitSleepMode();
    return 0;
}
int LCD_Drive_DeInit(void)
{
    Screen_EnterSleepMode();
    // 禁用 PWM 输出
    TIM_CtrlPWMOutputs(TIM1, DISABLE);
    // 禁用 DMA 通道
#if USE_DMA
    DMA_Cmd(DMA1_Channel3, DISABLE);                  // 禁用DMA通道
    NVIC_DisableIRQ(DMA1_Channel3_IRQn);              // 禁用DMA中断
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE); // 禁用SPI的DMA发送请求
#endif
    // 关闭 SPI
    SPI_Cmd(SPI1, DISABLE);                               // 禁用SPI外设
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE); // 禁用 SPI1 时钟

    return 0;
}
/******************************************************************************
function :  Initialize the lcd register
parameter:
******************************************************************************/

// 常量数组存储初始化命令和数据
static const uint8_t init_cmds[] = {
    0xB0, 0xC0,                                                                               // 设置电源控制寄存器，配置电源相关参数
    0xB2, 0x2F,                                                                               // 设置帧率控制寄存器，调整帧率或刷新率
    0xB3, 0x03,                                                                               // 设置显示模式或时序控制寄存器，调整显示模式
    0xB6, 0x19,                                                                               // 设置显示方向或扫描模式，调整显示方向
    0xB7, 0x01,                                                                               // 设置显示控制寄存器，开启或关闭某些显示功能
    0xAC, 0xCB,                                                                               // 设置电源控制或电压调节寄存器，调整电源或电压参数
    0xAB, 0x0E,                                                                               // 设置显示亮度或对比度控制寄存器，调整亮度或对比度
    0xB4, 0x04,                                                                               // 设置显示控制寄存器，调整显示控制参数
    0xA8, 0x19,                                                                               // 设置显示模式或时序控制寄存器，调整显示模式
    0x3A, 0x05,                                                                               // 设置像素格式，例如 RGB 565 或 RGB 666
    0xB8, 0x08,                                                                               // 设置显示控制寄存器，调整显示控制参数
    0xE8, 0x24,                                                                               // 设置电源控制或电压调节寄存器，调整电源或电压参数
    0xE9, 0x48,                                                                               // 设置电源控制或电压调节寄存器，调整电源或电压参数
    0xEA, 0x22,                                                                               // 设置电源控制或电压调节寄存器，调整电源或电压参数
    0xC6, 0x30,                                                                               // 设置显示控制寄存器，调整显示控制参数
    0xC7, 0x18,                                                                               // 设置显示控制寄存器，调整显示控制参数
    0xF0, 0x1F, 0x28, 0x04, 0x3E, 0x2A, 0x2E, 0x20, 0x00, 0x0C, 0x06, 0x00, 0x1C, 0x1F, 0x0F, // 设置 Gamma 校正寄存器，调整 Gamma 曲线
    0xF1, 0x00, 0x2D, 0x2F, 0x3C, 0x6F, 0x1C, 0x0B, 0x00, 0x00, 0x00, 0x07, 0x0D, 0x11, 0x0F, // 设置 Gamma 校正寄存器，调整 Gamma 曲线
    0x21,                                                                                     // 开启显示反转功能（Display Inversion ON）
    0x11,                                                                                     // 退出睡眠模式（Sleep Out），使 LCD 进入正常工作状态
    0x29                                                                                      // 开启显示（Display ON），使屏幕开始显示图像
};

static void Screen_InitReg(void)
{

    for (int8_t i = 0; i < sizeof(init_cmds);)
    {
        uint8_t cmd = init_cmds[i++];
        Screen_SendCommand(cmd);

        // 跳过无参数命令
        if (cmd == 0x21 || cmd == 0x11 || cmd == 0x29)
            continue;

        // 处理数据(1个或14个字节)
        uint8_t len = (cmd == 0xF0 || cmd == 0xF1) ? 14 : 1;
        while (len--)
            Screen_SendData_8Bit(init_cmds[i++]);
    }
}

void Screen_EnterSleepMode(void)
{
    // 发送睡眠模式命令 (0x10)
    Screen_SendCommand(0x10);

    // 根据数据手册，可能需要等待一段时间（例如 5ms）以确保命令生效
    // 具体等待时间请参考 LCD 控制器的数据手册
    Delay_Ms(5); // 使用 HAL 库的延时函数，延时 5 毫秒
}
void Screen_ExitSleepMode(void)
{
    // 发送退出睡眠模式命令 (0x11)
    Screen_SendCommand(0x11);

    // 根据数据手册，可能需要等待一段时间（例如 120ms）以确保显示稳定
    Delay_Ms(120); // 使用 HAL 库的延时函数，延时 120 毫秒
}
/********************************************************************************
function:   Set the resolution and scanning method of the screen
parameter:
        Scan_dir:   Scan direction
********************************************************************************/
static void Screen_SetAttributes(UBYTE Scan_dir)
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
    Screen_SendCommand(0x36);              // MX, MY, RGB mode
    Screen_SendData_8Bit(MemoryAccessReg); // 0x08 set RGB
}

static void Screen_Reset(void)
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
void Screen_Init(UBYTE Scan_dir)
{

    // Hardware reset
    Screen_Reset();

    // Set the resolution and scanning method of the screen
    Screen_SetAttributes(Scan_dir);

    // Set the initialization register
    Screen_InitReg();
}

/********************************************************************************
function:   Sets the start position and size of the display area
parameter:
        Xstart  :   X direction Start coordinates
        Ystart  :   Y direction Start coordinates
        Xend    :   X direction end coordinates
        Yend    :   Y direction end coordinates
********************************************************************************/
void Screen_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{

    dmaXoffset = 0;
    dmaYoffset = 0;

    Ystart = Ystart + 1;
    Yend = Yend + 1;

    Xstart = Xstart + 2;
    Xend = Xend + 2;

    // set the X coordinates
    Screen_SendCommand(0x2A);
    Screen_SendData_8Bit((Xstart >> 8) & 0xFF);
    Screen_SendData_8Bit(Xstart & 0xFF);
    Screen_SendData_8Bit(((Xend) >> 8) & 0xFF);
    Screen_SendData_8Bit((Xend) & 0xFF);

    // set the Y coordinates
    Screen_SendCommand(0x2B);
    Screen_SendData_8Bit((Ystart >> 8) & 0xFF);
    Screen_SendData_8Bit(Ystart & 0xFF);
    Screen_SendData_8Bit(((Yend) >> 8) & 0xFF);
    Screen_SendData_8Bit((Yend) & 0xFF);

    Screen_SendCommand(0X2C);
}

/******************************************************************************
function :  Clear screen
parameter:
******************************************************************************/
// 0到128
u8 Screen_Clear(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color)
{

#if USE_DMA
    while (!READ_LORA_CS)
    {
        DEBUG_PRINT("wait lora cs 1!\r\n");
    }
    int dma_circular = 0;
    u8 retry = 0;
    Screen_SetWindows(Xstart, Ystart, Xend + 0, Yend + 0);
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
    SPI_DMA_Tx_Init(DMA1_Channel3, (u32)&SPI1->DATAR, (u32)lcd_gram, Y_MAX_PIXEL * X_MAX_PIXEL * 2, DMA_Mode_Circular, DMA_MemoryInc_Enable);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA1_Channel3, ENABLE);

    // 计算总传输次数
    int total_dma_transfers = ((Yend - Ystart) * (Xend - Xstart) / (X_MAX_PIXEL * Y_MAX_PIXEL)) + 1;

    while (dma_circular <= total_dma_transfers)
    {
        if (dmaTransferComplete)
        {
            dmaTransferComplete = 0;
            dma_circular++;

            // 打印当前传输次数，帮助调试
            // DEBUG_PRINT("Current DMA transfers: %d/%d\r\n", dma_circular, total_dma_transfers);
        }
    }

    // 重置计数器
    dma_circular = 0;

    // 等待SPI发送缓冲区为空
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
        // 可以考虑在此处添加超时逻辑，以防死循环
        //  DEBUG_PRINT("Waiting for SPI transmit buffer to be empty...\r\n"); // 打印等待信息
    }

    // // 禁用DMA和SPI
     DMA_Cmd(DMA1_Channel3, DISABLE);
     SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
    DMA_ClearITPendingBit(DMA1_IT_TC3);

    LCD_CS_DISABLE;
    // DEBUG_PRINT("Screen_Clear OK\r\n"); // 等待SPI发送缓冲区为空

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

#else
    UWORD i, j;
    Screen_SetWindows(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    LCD_DC_1;
    for (i = 0; i < LCD_WIDTH; i++)
    {
        for (j = 0; j < LCD_HEIGHT; j++)
        {
            LCD_DC_1;
            LCD_CS_ENABLE;
            Screen_spi_write((Color >> 8) & 0xff);
            Screen_spi_write(Color & 0xff);
            LCD_CS_DISABLE;
        }
    }
#endif
}

/******************************************************************************
function :  Sends the image buffer in RAM to displays
parameter:
******************************************************************************/
void Screen_Display(UWORD *Image)
{
    UWORD i, j;
    Screen_SetWindows(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    LCD_DC_1;
    for (i = 0; i < LCD_WIDTH; i++)
    {
        for (j = 0; j < LCD_HEIGHT; j++)
        {
            Screen_spi_write((*(Image + i * LCD_WIDTH + j) >> 8) & 0xff);
            Screen_spi_write(*(Image + i * LCD_WIDTH + j));
        }
    }
}

void Screen_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image)
{
    // display
    UDOUBLE Addr = 0;

    UWORD i, j;
    Screen_SetWindows(Xstart, Ystart, Xend - 1, Yend - 1);
    LCD_DC_1;
    for (i = Ystart; i < Yend - 1; i++)
    {
        Addr = Xstart + i * LCD_WIDTH;
        for (j = Xstart; j < Xend - 1; j++)
        {
            Screen_spi_write((*(Image + Addr + j) >> 8) & 0xff);
            Screen_spi_write(*(Image + Addr + j));
        }
    }
}

u8 LCD_Refrsh_DMA(int pic_size)
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

    DMA_Cmd(DMA1_Channel3, DISABLE);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
    DMA_ClearITPendingBit(DMA1_IT_TC3);

    u8 retry = 0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    {
        retry++;
        if (retry > 200)
        {
            // DEBUG_PRINT("rec spi outtime!\r\n");
            return 0;
        }
    } // 还没收完

    LCD_CS_DISABLE;

    memset(lcd_gram, 0xff, pic_size);

    return (uint8_t)SPI_I2S_ReceiveData(SPI1);

#endif
}
/******************************************************************************
function: Draw a point
parameter   :
        X   :   Set the X coordinate
        Y   :   Set the Y coordinate
      Color :   Set the color
******************************************************************************/
void Screen_DrawPaint(UWORD x, UWORD y, UWORD Color)
{

    //
    //   DEBUG_PRINT("x = %d, y = %d,dmaXpoint:%d,dmaYpoint:%d\r\n",x,y,dmaXpoint,dmaYpoint);
    //      DEBUG_PRINT("dmaFont->Width:%d\r\n", dmaFont->Width);
    //

#if USE_DMA

    ///< 使用DMA的话，从对点刷屏到对显存数组写入数据，DMA传输数据的时候再统一进行传输
    int index = ((y - dmaYpoint) * (dmaFont->Width) + (x - dmaXpoint)) * 2; // 新的坐标过来转换成012345
    // DEBUG_PRINT("index开始偏移前:%d\r\n", index );

    index = index - X_MAX_PIXEL * Y_MAX_PIXEL * 2 * dmaXoffset / X_MAX_PIXEL; // 偏移
    lcd_gram[index] = (Color >> 8) & 0xff;                                    // 高字节
    lcd_gram[index + 1] = Color & 0xFF;                                       // 低字节
                                                                              //  DEBUG_PRINT("开始偏移:%d\r\n", index + 1);

    if ((index + 1) == (Y_MAX_PIXEL * X_MAX_PIXEL * 2 - 1))
    {

        LCD_Refrsh_DMA(Y_MAX_PIXEL * X_MAX_PIXEL * 2);
        dmaXoffset = X_MAX_PIXEL + dmaXoffset;
        dmaYoffset = Y_MAX_PIXEL + dmaYoffset;
        DEBUG_PRINT("dma ok\r\n");
    }

#else
    Screen_SetWindows(x, y, x, y);
    Screen_SendData_16Bit(Color);
#endif
}

/*******************************************************************************
function:
    Setting backlight
parameter   :
      value : Range 0~1000   Duty cycle is value/1000
*******************************************************************************/
void Screen_SetBackLight(UWORD Value)
{
    DEV_Set_PWM(Value);
}
