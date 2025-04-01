/*
 * Screen_api.h
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

#define Screen_SetBacklight(Value) DEV_SetBacklight(Value)



typedef struct{
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
}Screen_ATTRIBUTES;
extern Screen_ATTRIBUTES LCD;

extern u8 dmaXpoint,dmaYpoint;
extern sFONT* dmaFont;
extern volatile u8 dmaTransferComplete ;



/********************************************************************************
function:
            Macro definition variable name
********************************************************************************/
u8 Screen_Clear(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,UWORD Color);
u8 LCD_Refrsh_DMA(int pic_size);
void LCD_SHOW_API_INIT();
void Screen_Init(UBYTE Scan_dir);
void Screen_Display(UWORD *Image);
void Screen_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void Screen_DisplayPoint(UWORD X, UWORD Y, UWORD Color);
void Screen_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend);
void Screen_DrawPaint(UWORD x, UWORD y, UWORD Color);
void Screen_SetBackLight(UWORD Value);
void startup_animation();
void Screen_SendCommand(UBYTE Reg);
void Screen_EnterSleepMode(void);
void Screen_ExitSleepMode(void);
int LCD_Drive_DeInit(void);
int LCD_Drive_Init(void);

#endif /* SCREEN_SCREEN_API_H_ */













