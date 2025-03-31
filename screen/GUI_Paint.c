/******************************************************************************
 * | File      	:   GUI_Paint.c
 * | Author      :   Waveshare electronics
 * | Function    :	Achieve drawing: draw points, lines, boxes, circles and
 *                   their size, solid dotted line, solid rectangle hollow
 *                   rectangle, solid circle hollow circle.
 * | Info        :
 *   Achieve display characters: Display a single character, string, number
 *   Achieve time display: adaptive size display time minutes and seconds
 *----------------
 * |	This version:   V3.1
 * | Date        :   2020-08-15
 * | Info        :
 * -----------------------------------------------------------------------------
 * V3.1(2020-08-15):
 * 1.Fix:
 *       Paint_DrawNum
 *         Fixed a BUG where the Paint_DrawNum function failed to display 0
 * 2.Add： Paint_DrawFloatNum
 *     Can display FloatNum
 *
 * -----------------------------------------------------------------------------
 * V3.0(2019-04-18):
 * 1.Change:
 *    Paint_DrawPoint(..., DOT_STYLE DOT_STYLE)
 * => Paint_DrawPoint(..., DOT_STYLE Dot_Style)
 *    Paint_DrawLine(..., LINE_STYLE Line_Style, DOT_PIXEL Dot_Pixel)
 * => Paint_DrawLine(..., DOT_PIXEL Line_width, LINE_STYLE Line_Style)
 *    Paint_DrawRectangle(..., DRAW_FILL Filled, DOT_PIXEL Dot_Pixel)
 * => Paint_DrawRectangle(..., DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
 *    Paint_DrawCircle(..., DRAW_FILL Draw_Fill, DOT_PIXEL Dot_Pixel)
 * => Paint_DrawCircle(..., DOT_PIXEL Line_width, DRAW_FILL Draw_Filll)
 *
 * -----------------------------------------------------------------------------
 * V2.0(2018-11-15):
 * 1.add: Paint_NewImage()
 *    Create an image's properties
 * 2.add: Paint_SelectImage()
 *    Select the picture to be drawn
 * 3.add: Paint_SetRotate()
 *    Set the direction of the cache
 * 4.add: Paint_RotateImage()
 *    Can flip the picture, Support 0-360 degrees,
 *    but only 90.180.270 rotation is better
 * 4.add: Paint_SetMirroring()
 *    Can Mirroring the picture, horizontal, vertical, origin
 * 5.add: Paint_DrawString_CN()
 *    Can display Chinese(GB1312)
 *
 * -----------------------------------------------------------------------------
 * V1.0(2018-07-17):
 *   Create library
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

#include "GUI_Paint.h"
#include "drive_screen.h"
#include <stdint.h>
#include <stdlib.h>
// #include <string.h> //memset()
#include <math.h>

volatile PAINT Paint;
u8 dmaXpoint, dmaYpoint = 0; // 初始位置
sFONT *dmaFont;
// void (*DISPLAY)(UWORD, UWORD, UWORD);
// void (*CLEAR)(UWORD);
/******************************************************************************
function:	Create Image
parameter:
    image   :   Pointer to the image cache
    width   :   The width of the picture
    Height  :   The height of the picture
    Color   :   Whether the picture is inverted
******************************************************************************/
void Paint_NewImage(UWORD Width, UWORD Height, UWORD Rotate, UWORD Color)
{
    Paint.WidthMemory = Width;
    Paint.HeightMemory = Height;
    Paint.Color = Color;
    Paint.WidthByte = Width;
    Paint.HeightByte = Height;
    // DEBUG_PRINT("WidthByte = %d, HeightByte = %d\r\n", Paint.WidthByte, Paint.HeightByte);

    Paint.Rotate = Rotate;
    Paint.Mirror = MIRROR_NONE;

    if (Rotate == ROTATE_0 || Rotate == ROTATE_180)
    {
        Paint.Width = Width;
        Paint.Height = Height;
    }
    else
    {
        Paint.Width = Height;
        Paint.Height = Width;
    }
}
// /******************************************************************************
// function:	Select Clear Funtion
// parameter:
//       Clear :   Pointer to Clear funtion
// ******************************************************************************/
// void Paint_SetClearFuntion(void (*Clear)(UWORD))
// {
//     CLEAR = Clear;
// }
/******************************************************************************
function:	Select DisplayF untion
parameter:
      Display :   Pointer to display funtion
******************************************************************************/
// void Paint_SetDisplayFuntion(void (*Display)(UWORD, UWORD, UWORD))
// {
//     DISPLAY = Display;
// }

/******************************************************************************
function:	Select Image Rotate
parameter:
    Rotate   :   0,90,180,270
******************************************************************************/
void Paint_SetRotate(UWORD Rotate)
{
    if (Rotate == ROTATE_0 || Rotate == ROTATE_90 || Rotate == ROTATE_180 || Rotate == ROTATE_270)
    {
        // DEBUG_PRINT("Set image Rotate %d\r\n", Rotate);
        Paint.Rotate = Rotate;
    }
    else
    {
        // DEBUG_PRINT("rotate = 0, 90, 180, 270\r\n");
        //   exit(0);
    }
}

/******************************************************************************
function:	Select Image mirror   设置图像的镜像效果
parameter:
mirror   :       Not mirror,Horizontal mirror,Vertical mirror,Origin mirror
******************************************************************************/
void Paint_SetMirroring(UBYTE mirror)
{
    if (mirror == MIRROR_NONE || mirror == MIRROR_HORIZONTAL ||
        mirror == MIRROR_VERTICAL || mirror == MIRROR_ORIGIN)
    {
        //  DEBUG_PRINT("mirror image x:%s, y:%s\r\n", (mirror & 0x01) ? "mirror" : "none", ((mirror >> 1) & 0x01) ? "mirror" : "none");
        Paint.Mirror = mirror;
    }
    else
    {
        //  DEBUG_PRINT("mirror should be MIRROR_NONE, MIRROR_HORIZONTAL, \
        MIRROR_VERTICAL or MIRROR_ORIGIN\r\n");
        // exit(0);
    }
}

/******************************************************************************
function:	Draw Pixels
parameter:
    Xpoint  :   At point X
    Ypoint  :   At point Y
    Color   :   Painted colors
******************************************************************************/
void Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color)
{
    // if (Xpoint > Paint.Width || Ypoint > Paint.Height)
    // {
    //     DEBUG_PRINT("Paint_SetPixel1 erro %d,%d\r\n", Xpoint, Ypoint);
    //     return;
    // }
    UWORD X, Y;

    switch (Paint.Rotate)
    {
    case 0:
        X = Xpoint;
        Y = Ypoint;
        break;
    case 90:
        X = Paint.WidthMemory - Ypoint - 1;
        Y = Xpoint;
        break;
    case 180:
        X = Paint.WidthMemory - Xpoint - 1;
        Y = Paint.HeightMemory - Ypoint - 1;
        break;
    case 270:
        X = Ypoint;
        Y = Paint.HeightMemory - Xpoint - 1;
        break;

    default:
        return;
    }

    // switch (Paint.Mirror)
    // {
    // case MIRROR_NONE:
    //     break;
    // case MIRROR_HORIZONTAL:
    //     X = Paint.WidthMemory - X - 1;
    //     break;
    // case MIRROR_VERTICAL:
    //     Y = Paint.HeightMemory - Y - 1;
    //     break;
    // case MIRROR_ORIGIN:
    //     X = Paint.WidthMemory - X - 1;
    //     Y = Paint.HeightMemory - Y - 1;
    //     break;
    // default:
    //     return;
    // }

    // if (X > Paint.WidthMemory || Y > Paint.HeightMemory)
    // {
    //     DEBUG_PRINT("Paint_SetPixel2 erro,x = %d, y = %d\r\n", X, Y);
    //     return;
    // }

    // UDOUBLE Addr = X / 8 + Y * Paint.WidthByte;
    Screen_DrawPaint(X, Y, Color);
}

/******************************************************************************
function:	Clear the color of a window
parameter:
    Xstart :   x starting point
    Ystart :   Y starting point
    Xend   :   x end point
    Yend   :   y end point
******************************************************************************/
void Paint_ClearWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color)
{
    UWORD X, Y;
    for (Y = Ystart; Y < Yend; Y++)
    {
        for (X = Xstart; X < Xend; X++)
        { // 8 pixel =  1 byte
            Paint_SetPixel(X, Y, Color);
        }
    }
}

/******************************************************************************
function:	Draw Point(Xpoint, Ypoint) Fill the color
parameter:
    Xpoint		:   The Xpoint coordinate of the point
    Ypoint		:   The Ypoint coordinate of the point
    Color		:   Set color
    Dot_Pixel	:	point size
******************************************************************************/
void Paint_DrawPoint(UWORD Xpoint, UWORD Ypoint, UWORD Color, DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_FillWay)
{
//    if (Xpoint >= Paint.Width || Ypoint >= Paint.Height)
//    {
//        DEBUG_PRINT("Paint_DrawPoint erro,Xpoint = %d, Ypoint = %d\r\n", Xpoint, Ypoint);
//        return;
//    }

    int16_t XDir_Num, YDir_Num;
    if (Dot_FillWay == DOT_FILL_AROUND)
    {
        for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++)
        {
            for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++)
            {
                int16_t x = Xpoint + XDir_Num - Dot_Pixel + 1;
                int16_t y = Ypoint + YDir_Num - Dot_Pixel + 1;

                // 边界检查
                if (x >= 0 && y >= 0 && x < Paint.Width && y < Paint.Height)
                {
                    Paint_SetPixel(x, y, Color);
                }
            }
        }
    }
    // else
    // {
    //     for (XDir_Num = 0; XDir_Num < Dot_Pixel; XDir_Num++)
    //     {
    //         for (YDir_Num = 0; YDir_Num < Dot_Pixel; YDir_Num++)
    //         {
    //             int16_t x = Xpoint + XDir_Num;
    //             int16_t y = Ypoint + YDir_Num;

    //             // 边界检查
    //             if (x >= 0 && y >= 0 && x < Paint.Width && y < Paint.Height)
    //             {
    //                 Paint_SetPixel(x, y, Color);
    //             }
    //         }
    //     }
    // }
}

/******************************************************************************
function:	Draw a line of arbitrary slope
parameter:
    Xstart ：Starting Xpoint point coordinates
    Ystart ：Starting Xpoint point coordinates
    Xend   ：End point Xpoint coordinate
    Yend   ：End point Ypoint coordinate
    Color  ：The color of the line segment
******************************************************************************/
void Paint_DrawLine(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,
                    UWORD Color, DOT_PIXEL Line_width, LINE_STYLE Line_Style)
{
    // if (Xstart > Paint.Width || Ystart > Paint.Height ||
    //     Xend > Paint.Width || Yend > Paint.Height)
    // {
    //     DEBUG_PRINT("Paint_DrawLine erro,Xstart = %d, Ystart = %d,Xend = %d, Yend = %d\r\n", Xstart, Ystart, Xend, Yend);

    //     return;
    // }

    UWORD Xpoint = Xstart;
    UWORD Ypoint = Ystart;

    int8_t dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend; // 0
    int8_t dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend; //-16

    // Increment direction, 1 is positive, -1 is counter;
    int8_t XAddway = Xstart < Xend ? 1 : -1; //-1
    int8_t YAddway = Ystart < Yend ? 1 : -1; // 1

    // Cumulative error
    int16_t Esp = dx + dy; //-16
    uint8_t Dotted_Len = 0;

#if USE_DMA

    dmaXpoint = Xpoint;
    dmaYpoint = Ypoint;
    dmaFont = &Fontline;

    Screen_SetWindows(Xstart, Ystart, Xend, Yend); // 准备好窗口和复位
    // DEBUG_PRINT("Xstart, Ystart, Xend, Yend = %d= %d= %d= %d\r\n", Xstart, Ystart, Xend, Yend);
    // DEBUG_PRINT("wwwwdmaFont->Width:%d\r\n", dmaFont->Width);

#endif

    for (;;)
    {
        Dotted_Len++;
        // Painted dotted line, 2 point is really virtual
        if (Line_Style == LINE_STYLE_DOTTED && Dotted_Len % 3 == 0)
        {
            // DEBUG_PRINT("LINE_DOTTED\r\n");
            // Paint_DrawPoint(Xpoint, Ypoint, IMAGE_BACKGROUND, Line_width, DOT_STYLE_DFT);
            // Dotted_Len = 0;
        }
        else
        {
            Paint_DrawPoint(Xpoint, Ypoint, Color, Line_width, DOT_STYLE_DFT); // 44,45,46,47,48
        }
        if (2 * Esp >= dy) // 8>4
        {
            if (Xpoint == Xend)
                break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if (2 * Esp <= dx) // 跑这里
        {
            // DEBUG_PRINT("2 * Esp = %d\r\n", 2 * Esp );
            if (Ypoint == Yend)
                break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
    // DEBUG_PRINT("Dotted_Len = %d\r\n", Dotted_Len);

#if USE_DMA
    LCD_Refrsh_DMA(Dotted_Len * 2);
#endif
}

/******************************************************************************
function:	Draw a rectangle
parameter:
    Xstart ：Rectangular  Starting Xpoint point coordinates
    Ystart ：Rectangular  Starting Xpoint point coordinates
    Xend   ：Rectangular  End point Xpoint coordinate
    Yend   ：Rectangular  End point Ypoint coordinate
    Color  ：The color of the Rectangular segment
    Filled : Whether it is filled--- 1 solid 0：empty
    输入0到127
******************************************************************************/
void Paint_DrawRectangle(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,
                         UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Filled)
{
//    if (Xstart > Paint.Width || Ystart > Paint.Height ||
//        Xend > Paint.Width || Yend > Paint.Height)
//    {
//        DEBUG_PRINT("Paint_DrawRectangle erro,Xstart = %d, Ystart = %d,Xend = %d, Yend = %d\r\n", Xstart, Ystart, Xend, Yend);
//
//        return;
//    }

    if (Filled)
    {
        UWORD Ypoint;
        for (Ypoint = Ystart; Ypoint <= Yend; Ypoint++)
        {
            Paint_DrawLine(Xstart, Ypoint, Xend, Ypoint, Color, Line_width, LINE_STYLE_SOLID);
        }
    }
    else
    {
        Paint_DrawLine(Xstart, Ystart, Xend, Ystart, Color, Line_width, LINE_STYLE_SOLID);
        Paint_DrawLine(Xstart, Ystart, Xstart, Yend, Color, Line_width, LINE_STYLE_SOLID);
        Paint_DrawLine(Xend, Ystart, Xend, Yend, Color, Line_width, LINE_STYLE_SOLID);
        Paint_DrawLine(Xstart, Yend, Xend, Yend, Color, Line_width, LINE_STYLE_SOLID);
    }
}

/******************************************************************************
function:	Use the 8-point method to draw a circle of the
            specified size at the specified position->
parameter:
    X_Center  ：Center X coordinate
    Y_Center  ：Center Y coordinate
    Radius    ：circle Radius
    Color     ：The color of the ：circle segment
    Filled    : Whether it is filled: 1 filling 0：Do not
******************************************************************************/
void Paint_DrawCircle(UWORD X_Center, UWORD Y_Center, UWORD Radius,
                      UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
{
    // if (X_Center > Paint.Width || Y_Center >= Paint.Height)
    // {
    //     DEBUG_PRINT("Paint_DrawCircle erro,X_Center = %d, Y_Center = %d\r\n", X_Center, Y_Center);

    //     return;
    // }

    // Draw a circle from(0, R) as a starting point
    int16_t XCurrent, YCurrent;
    XCurrent = 0;
    YCurrent = Radius;

    // Cumulative error,judge the next point of the logo
    int16_t Esp = 3 - (Radius << 1);

    int16_t sCountY;
    if (Draw_Fill == DRAW_FILL_FULL)
    {

        while (XCurrent <= YCurrent)
        { // Realistic circles
            for (sCountY = XCurrent; sCountY <= YCurrent; sCountY++)
            {
                Paint_DrawPoint(X_Center + XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 1
                Paint_DrawPoint(X_Center - XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 2
                Paint_DrawPoint(X_Center - sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 3
                Paint_DrawPoint(X_Center - sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 4
                Paint_DrawPoint(X_Center - XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 5
                Paint_DrawPoint(X_Center + XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 6
                Paint_DrawPoint(X_Center + sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 7
                Paint_DrawPoint(X_Center + sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            }
            if (Esp < 0)
                Esp += 4 * XCurrent + 6;
            else
            {
                Esp += 10 + 4 * (XCurrent - YCurrent);
                YCurrent--;
            }
            XCurrent++;
        }
    }
    else
    { // Draw a hollow circle
        while (XCurrent <= YCurrent)
        {
            Paint_DrawPoint(X_Center + XCurrent, Y_Center + YCurrent, Color, Line_width, DOT_STYLE_DFT); // 1
            Paint_DrawPoint(X_Center - XCurrent, Y_Center + YCurrent, Color, Line_width, DOT_STYLE_DFT); // 2
            Paint_DrawPoint(X_Center - YCurrent, Y_Center + XCurrent, Color, Line_width, DOT_STYLE_DFT); // 3
            Paint_DrawPoint(X_Center - YCurrent, Y_Center - XCurrent, Color, Line_width, DOT_STYLE_DFT); // 4
            Paint_DrawPoint(X_Center - XCurrent, Y_Center - YCurrent, Color, Line_width, DOT_STYLE_DFT); // 5
            Paint_DrawPoint(X_Center + XCurrent, Y_Center - YCurrent, Color, Line_width, DOT_STYLE_DFT); // 6
            Paint_DrawPoint(X_Center + YCurrent, Y_Center - XCurrent, Color, Line_width, DOT_STYLE_DFT); // 7
            Paint_DrawPoint(X_Center + YCurrent, Y_Center + XCurrent, Color, Line_width, DOT_STYLE_DFT); // 0

            if (Esp < 0)
                Esp += 4 * XCurrent + 6;
            else
            {
                Esp += 10 + 4 * (XCurrent - YCurrent);
                YCurrent--;
            }
            XCurrent++;
        }
    }
}

/******************************************************************************
function:	Show English characters
parameter:
    Xpoint           ：X coordinate
    Ypoint           ：Y coordinate
    Acsii_Char       ：To display the English characters
    Font             ：A structure pointer that displays a character size
    Color_Background : Select the background color of the English character
    Color_Foreground : Select the foreground color of the English character
******************************************************************************/

void Paint_DrawChar(UWORD Xpoint, UWORD Ypoint, const char Acsii_Char,
                    sFONT *Font, UWORD Color_Background, UWORD Color_Foreground,
                    const char offsetAcsii)
{
    // 局部变量
    UWORD Page, Column;
    uint32_t Char_Offset;
    
    // 为Font16_En字体使用映射表查找真实的偏移
    extern const uint8_t Font16_english_map[];
    extern sFONT Font16_En;
    
    if (Font == &Font16_En) {
        // 查找字符在映射表中的位置
        int map_index = -1;
        for (int i = 0; i < 16; i++) {  // 目前映射表有16个字符
            if (Font16_english_map[i] == Acsii_Char) {
                map_index = i;
                break;
            }
        }
        
        if (map_index != -1) {
            // 找到了映射的字符，使用映射索引
            Char_Offset = map_index * Font->Height * ((Font->Width + 7) / 8);
        } else {
            // 未找到映射，默认使用空格字符(使用第一个字符的偏移)
            Char_Offset = 0;
        }
    } else {
        // 其他字体使用普通偏移计算
        Char_Offset = (Acsii_Char - offsetAcsii) * Font->Height * ((Font->Width + 7) / 8);
    }

    const unsigned char *ptr = &Font->table[Char_Offset];

    // 检查坐标范围
    if (Xpoint > Paint.Width || Ypoint > Paint.Height)
    {
        DEBUG_PRINT("Paint_DrawChar error, Xpoint = %d, Ypoint = %d\r\n", Xpoint, Ypoint);
        return;
    }

#if USE_DMA

    dmaXpoint = Xpoint;
    dmaYpoint = Ypoint;
    dmaFont = Font;

    Screen_SetWindows(Xpoint, Ypoint, (Xpoint + Font->Width) - 1, (Ypoint + Font->Height) - 1); // 准备窗口
#endif

    // 遍历字符的每一行和每一列
    for (Page = 0; Page < Font->Height; Page++)
    {
        for (Column = 0; Column < Font->Width; Column++)
        {
            // 计算当前像素的位
            uint8_t pixel_bit = *ptr & (0x80 >> (Column % 8));
            if (pixel_bit)
            {
                Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
            }
            else if (Color_Background != FONT_BACKGROUND) // 如果背景色不同，绘制背景色
            {
                Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Background);
            }

            // 每8列更新一次指针
            if (Column % 8 == 7)
                ptr++;
        }
        // 每行结束后可能需要更新指针
        if (Font->Width % 8 != 0)
            ptr++;
    }

#if USE_DMA
    int total_pixels = dmaFont->Width * dmaFont->Height * 2;
    if (total_pixels < Y_MAX_PIXEL * X_MAX_PIXEL * 2)
    {
        LCD_Refrsh_DMA(total_pixels); // 刷新小图
    }
    if (total_pixels % (Y_MAX_PIXEL * X_MAX_PIXEL * 2) && total_pixels > Y_MAX_PIXEL * X_MAX_PIXEL * 2)
    {
        LCD_Refrsh_DMA(total_pixels % (Y_MAX_PIXEL * X_MAX_PIXEL * 2)); // 补包操作
    }
#endif
}

void Paint_Drawicon(UWORD Xpoint, UWORD Ypoint, u8 number,
                    sFONT *Font, UWORD Color_Background, UWORD Color_Foreground)
{

    UWORD Page, Column;
    if (Xpoint > Paint.Width || Ypoint > Paint.Height)
    {

        DEBUG_PRINT("Paint_Drawicon erro,Xpoint = %d, Ypoint = %d\r\n", Xpoint, Ypoint);

        return;
    }
    dmaXpoint = Xpoint;
    dmaYpoint = Ypoint;
    dmaFont = Font;
    uint32_t Char_Offset = number * Font->Height * (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
    const unsigned char *ptr = &Font->table[Char_Offset];

#if USE_DMA

    Screen_SetWindows(Xpoint, Ypoint, (Xpoint + Font->Width) - 1, (Ypoint + Font->Height) - 1); // 准备好窗口和复位
#endif

    for (Page = 0; Page < Font->Height; Page++)
    {
        for (Column = 0; Column < Font->Width; Column++)
        {
            // DEBUG_PRINT("nwo:%08x\r\n", DBGMCU_GetCHIPID());

            // To determine whether the font background color and screen background color is consistent
            if (FONT_BACKGROUND == Color_Background)
            { // this process is to speed up the scan
                if (*ptr & (0x80 >> (Column % 8)))
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
                // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            }
            else
            {
                if (*ptr & (0x80 >> (Column % 8)))
                {
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
                    // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
                else
                {
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Background);
                    // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
            }
            // One pixel is 8 bits
            if (Column % 8 == 7)
                ptr++;
        } // Write a line
        if (Font->Width % 8 != 0)
            ptr++;

    } // Write all

#if USE_DMA
    // DEBUG_PRINT("显示小图%d\r\n",dmaFont->Width * dmaFont->Height * 2 / Y_MAX_PIXEL * X_MAX_PIXEL * 2);
    if (dmaFont->Width * dmaFont->Height * 2 < Y_MAX_PIXEL * X_MAX_PIXEL * 2)
    {
        // DEBUG_PRINT("显示小图\r\n");
        LCD_Refrsh_DMA(dmaFont->Width * dmaFont->Height * 2);
    }
    if (dmaFont->Width * dmaFont->Height * 2 % Y_MAX_PIXEL * X_MAX_PIXEL * 2) // 补包操作
    {

        LCD_Refrsh_DMA(dmaFont->Width * dmaFont->Height * 2 % Y_MAX_PIXEL * X_MAX_PIXEL * 2); // 把余数显示掉
    }

#endif
}

/******************************************************************************
function:	Display the string
parameter:
    Xstart           ：X coordinate
    Ystart           ：Y coordinate
    pString          ：The first address of the English string to be displayed
    Font             ：A structure pointer that displays a character size
    Color_Background : Select the background color of the English character
    Color_Foreground : Select the foreground color of the English character
******************************************************************************/
// 数字的偏移为'/'，英文的偏移为'`'
void Paint_DrawString(UWORD Xstart, UWORD Ystart, const char *pString,
                      sFONT *Font, UWORD Color_Background, UWORD Color_Foreground, const char offsetAcsii, UWORD changeColorNumber)
{
    UWORD Xpoint = Xstart;
    UWORD Ypoint = Ystart;
    UWORD position = 0;
    // if (Xstart > Paint.Width || Ystart > Paint.Height)
    // {
    //     DEBUG_PRINT("Paint_DrawString erro,Xstart = %d, Ystart = %d\r\n", Xstart, Ystart);

    //     return;
    // }

    while (*pString != '\0')
    {

        //    DEBUG_PRINT("needshow%c\r\n",*pString);
        // if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the Height of the character
        if ((Xpoint + Font->Width) > Paint.Width - 1)
        {
            Xpoint = Xstart;
            Ypoint += Font->Height;
        }

        // If the Y direction is full, reposition to(Xstart, Ystart)
        if ((Ypoint + Font->Height) > Paint.Height)
        {
            Xpoint = Xstart;
            Ypoint = Ystart;
        }

        if (Font->Height == 12 &&position == changeColorNumber)
            Paint_DrawChar(Xpoint, Ypoint, *pString, Font, RED, Color_Foreground, offsetAcsii);
        else
            Paint_DrawChar(Xpoint, Ypoint, *pString, Font, Color_Background, Color_Foreground, offsetAcsii);
        // The next character of the address
        pString++;
        position++;

        // The next word of the abscissa increases the font of the broadband
        Xpoint += Font->Width;
    }
}

/******************************************************************************
function:	Display the string
parameter:
    Xstart           ：X coordinate
    Ystart           ：Y coordinate
    pString          ：The first address of the Chinese string and English
                        string to be displayed
    Font             ：A structure pointer that displays a character size
    Color_Background : Select the background color of the English character
    Color_Foreground : Select the foreground color of the English character
******************************************************************************/
void Paint_DrawString_CN(UWORD Xstart, UWORD Ystart, const char *pString, cFONT *font, UWORD Color_Background, UWORD Color_Foreground)
{
    const char *p_text = pString;
    u8 x = Xstart, y = Ystart;
    u8 i, j, Num;

    /* Send the string character by character on EPD */
    while (*p_text != 0)
    {
        if (*p_text <= 0x7F)
        { // ASCII < 126
            for (Num = 0; Num < font->size; Num++)
            {
                if (*p_text == font->table[Num].index[0])
                {
                    const char *ptr = &font->table[Num].matrix[0];

                    for (j = 0; j < font->Height; j++)
                    {
                        for (i = 0; i < font->Width; i++)
                        {
                            if (FONT_BACKGROUND == Color_Background)
                            { // this process is to speed up the scan
                                if (*ptr & (0x80 >> (i % 8)))
                                {
                                    Paint_SetPixel(x + i, y + j, Color_Foreground);
                                    // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                                }
                            }
                            else
                            {
                                if (*ptr & (0x80 >> (i % 8)))
                                {
                                    Paint_SetPixel(x + i, y + j, Color_Foreground);
                                    // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                                }
                                else
                                {
                                    Paint_SetPixel(x + i, y + j, Color_Background);
                                    // Paint_DrawPoint(x + i, y + j, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                                }
                            }
                            if (i % 8 == 7)
                            {
                                ptr++;
                            }
                        }
                        if (font->Width % 8 != 0)
                        {
                            ptr++;
                        }
                    }
                    break;
                }
            }
            /* Point on the next character */
            p_text += 1;
            /* Decrement the column position by 16 */
            x += font->ASCII_Width;
        }
        else
        { // Chinese
            for (Num = 0; Num < font->size; Num++)
            {
                if ((*p_text == font->table[Num].index[0]) && (*(p_text + 1) == font->table[Num].index[1]))
                {
                    const char *ptr = &font->table[Num].matrix[0];

                    for (j = 0; j < font->Height; j++)
                    {
                        for (i = 0; i < font->Width; i++)
                        {
                            if (FONT_BACKGROUND == Color_Background)
                            { // this process is to speed up the scan
                                if (*ptr & (0x80 >> (i % 8)))
                                {
                                    Paint_SetPixel(x + i, y + j, Color_Foreground);
                                    // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                                }
                            }
                            else
                            {
                                if (*ptr & (0x80 >> (i % 8)))
                                {
                                    Paint_SetPixel(x + i, y + j, Color_Foreground);
                                    // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                                }
                                else
                                {
                                    Paint_SetPixel(x + i, y + j, Color_Background);
                                    // Paint_DrawPoint(x + i, y + j, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                                }
                            }
                            if (i % 8 == 7)
                            {
                                ptr++;
                            }
                        }
                        if (font->Width % 8 != 0)
                        {
                            ptr++;
                        }
                    }
                    break;
                }
            }
            /* Point on the next character */
            p_text += 2;
            /* Decrement the column position by 16 */
            x += font->Width;
        }
    }
}

/******************************************************************************
function:	Display nummber
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    Nummber          : The number displayed
    Font             ：A structure pointer that displays a character size
    Color_Background : Select the background color of the English character
    Color_Foreground : Select the foreground color of the English character
******************************************************************************/
#define ARRAY_LEN 255
void Paint_DrawNum(UWORD Xpoint, UWORD Ypoint, int32_t Nummber,
                   sFONT *Font, UWORD Color_Background, UWORD Color_Foreground)
{
    int16_t Num_Bit = 0, Str_Bit = 0;
    uint8_t Str_Array[ARRAY_LEN] = {0}, Num_Array[ARRAY_LEN] = {0};
    uint8_t *pStr = Str_Array;

    // if (Xpoint > Paint.Width || Ypoint > Paint.Height)
    // {

    //     DEBUG_PRINT("Paint_DrawNum erro %d,%d\r\n", Xpoint, Ypoint);

    //     return;
    // }

    // Converts a number to a string
    do
    {
        Num_Array[Num_Bit] = Nummber % 10 + '0';
        Num_Bit++;
        Nummber /= 10;
    } while (Nummber);

    // The string is inverted
    while (Num_Bit > 0)
    {
        Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
        Str_Bit++;
        Num_Bit--;
    }

    // show
    Paint_DrawString(Xpoint, Ypoint, (const char *)pStr, Font, Color_Background, Color_Foreground, ' ',999);
}
// /******************************************************************************
// function:	Display float number
// parameter:
//     Xstart           ：X coordinate
//     Ystart           : Y coordinate
//     Nummber          : The float data that you want to display
//     Decimal_Point	 : Show decimal places
//     Font             ：A structure pointer that displays a character size
//     Color            : Select the background color of the English character
// ******************************************************************************/
// void Paint_DrawFloatNum(UWORD Xpoint, UWORD Ypoint, double Nummber, UBYTE Decimal_Point,
//                         sFONT *Font, UWORD Color_Background, UWORD Color_Foreground)
// {
//     char Str[ARRAY_LEN];
//     sprintf(Str, "%.*lf", Decimal_Point + 2, Nummber);
//     char *pStr = (char *)malloc((strlen(Str)) * sizeof(char));
//     memcpy(pStr, Str, (strlen(Str) - 2));
//     *(pStr + strlen(Str) - 1) = '\0';
//     if ((*(pStr + strlen(Str) - 3)) == '.')
//     {
//         *(pStr + strlen(Str) - 3) = '\0';
//     }
//     // show
//     Paint_DrawString(Xpoint, Ypoint, (const char *)pStr, Font, Color_Foreground, Color_Background, ' ',999);
//     free(pStr);
//     pStr = NULL;
// }
/******************************************************************************
function:	Display time
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    pTime            : Time-related structures
    Font             ：A structure pointer that displays a character size
    Color            : Select the background color of the English character
******************************************************************************/
void Paint_DrawTime(UWORD Xstart, UWORD Ystart, PAINT_TIME *pTime, sFONT *Font,
                    UWORD Color_Background, UWORD Color_Foreground)
{
    uint8_t value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    UWORD Dx = Font->Width;

    // Write data into the cache
    Paint_DrawChar(Xstart, Ystart, value[pTime->Hour / 10], Font, Color_Background, Color_Foreground, ' ');
    Paint_DrawChar(Xstart + Dx, Ystart, value[pTime->Hour % 10], Font, Color_Background, Color_Foreground, ' ');
    Paint_DrawChar(Xstart + Dx + Dx / 4 + Dx / 2, Ystart, ':', Font, Color_Background, Color_Foreground, ' ');
    Paint_DrawChar(Xstart + Dx * 2 + Dx / 2, Ystart, value[pTime->Min / 10], Font, Color_Background, Color_Foreground, ' ');
    Paint_DrawChar(Xstart + Dx * 3 + Dx / 2, Ystart, value[pTime->Min % 10], Font, Color_Background, Color_Foreground, ' ');
    Paint_DrawChar(Xstart + Dx * 4 + Dx / 2 - Dx / 4, Ystart, ':', Font, Color_Background, Color_Foreground, ' ');
    Paint_DrawChar(Xstart + Dx * 5, Ystart, value[pTime->Sec / 10], Font, Color_Background, Color_Foreground, ' ');
    Paint_DrawChar(Xstart + Dx * 6, Ystart, value[pTime->Sec % 10], Font, Color_Background, Color_Foreground, ' ');
}

/******************************************************************************
function:	Display image
parameter:
    image            ：Image start address
    xStart           : X starting coordinates
    yStart           : Y starting coordinates
    xEnd             ：Image width
    yEnd             : Image height
******************************************************************************/
void Paint_DrawImage(const unsigned char *image, UWORD xStart, UWORD yStart, UWORD W_Image, UWORD H_Image)
{
    int i, j;
    for (j = 0; j < H_Image; j++)
    {
        for (i = 0; i < W_Image; i++)
        {
            if (xStart + i < Paint.WidthMemory && yStart + j < Paint.HeightMemory) // Exceeded part does not display
                Paint_SetPixel(xStart + i, yStart + j, (*(image + j * W_Image * 2 + i * 2 + 1)) << 8 | (*(image + j * W_Image * 2 + i * 2)));
            ////使用数组是顺序存储的一个特性，通过算法访问原始数组
            // j*W_Image*2 			   Y offset
            // i*2              	   X offset
        }
    }
}
