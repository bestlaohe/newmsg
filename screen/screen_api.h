/*
 * screen_api.h
 *
 *  Created on: 2024Äê8ÔÂ30ÈÕ
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


#define HORIZONTAL 0
#define VERTICAL   1

#define LCD_0IN85_SetBacklight(Value) DEV_SetBacklight(Value)



typedef struct{
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
}LCD_0IN85_ATTRIBUTES;
extern LCD_0IN85_ATTRIBUTES LCD;

/********************************************************************************
function:
            Macro definition variable name
********************************************************************************/
void LCD_0in85_test();
void LCD_0IN85_Init(UBYTE Scan_dir);
void LCD_0IN85_Clear(UWORD Color);
void LCD_0IN85_Display(UWORD *Image);
void LCD_0IN85_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_0IN85_DisplayPoint(UWORD X, UWORD Y, UWORD Color);
void LCD_0IN85_DrawPaint(UWORD x, UWORD y, UWORD Color);
void LCD_0IN85_SetBackLight(UWORD Value);
#endif /* SCREEN_SCREEN_API_H_ */













