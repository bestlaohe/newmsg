/*
 * screen_api.h
 *
 *  Created on: 2024年8月30日
 *      Author: 12630
 */

#ifndef SCREEN_SCREEN_API_H_
#define SCREEN_SCREEN_API_H_

#include "drive_screen.h"
#include "GUI_Paint.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


#define LCD_HEIGHT 128
#define LCD_WIDTH 128

#define USE_DMA 1


//768字节
#define X_MAX_PIXEL 24
#define Y_MAX_PIXEL 24

#define HORIZONTAL 0
#define VERTICAL   1

#define LCD_0IN85_SetBacklight(Value) DEV_SetBacklight(Value)



typedef struct{
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
}LCD_0IN85_ATTRIBUTES;
extern LCD_0IN85_ATTRIBUTES LCD;

extern u8 dmaXpoint,dmaYpoint;
extern sFONT* dmaFont;
extern volatile u8 dmaTransferComplete ;



/********************************************************************************
function:
            Macro definition variable name
********************************************************************************/
u8 LCD_0IN85_Clear(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,UWORD Color);
u8 LCD_Refrsh_DMA(int pic_size);
void LCD_SHOW_API_INIT();
void LCD_0IN85_Init(UBYTE Scan_dir);
void LCD_0IN85_Display(UWORD *Image);
void LCD_0IN85_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_0IN85_DisplayPoint(UWORD X, UWORD Y, UWORD Color);
void LCD_0IN85_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend);
void LCD_0IN85_DrawPaint(UWORD x, UWORD y, UWORD Color);
void LCD_0IN85_SetBackLight(UWORD Value);
void startup_animation();
void LCD_0IN85_SendCommand(UBYTE Reg);
void LCD_0IN85_EnterSleepMode(void);
void LCD_0IN85_ExitSleepMode(void);
int LCD_Drive_DeInit(void);
int LCD_Drive_Init(void);

#endif /* SCREEN_SCREEN_API_H_ */













