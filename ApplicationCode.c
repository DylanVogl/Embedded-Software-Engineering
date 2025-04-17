/*
 * ApplicationCode.c
 *
 *  Created on: Dec 30, 2023 (updated 11/12/2024) Thanks Donavon! 
 *      Author: Xavion
 */

#include "ApplicationCode.h"

/* Static variables */


extern void initialise_monitor_handles(void); 

#if COMPILE_TOUCH_FUNCTIONS == 1
static STMPE811_TouchData StaticTouchData;
#endif // COMPILE_TOUCH_FUNCTIONS

void ApplicationInit(void)
{
	initialise_monitor_handles(); // Allows printf functionality
    LTCD__Init();
    LTCD_Layer_Init(0);
    LCD_Clear(0,LCD_COLOR_WHITE);

    #if COMPILE_TOUCH_FUNCTIONS == 1
	InitializeLCDTouch();

	// This is the orientation for the board to be direclty up where the buttons are vertically above the screen
	// Top left would be low x value, high y value. Bottom right would be low x value, low y value.
	StaticTouchData.orientation = STMPE811_Orientation_Portrait_2;

	#endif // COMPILE_TOUCH_FUNCTIONS
}

void LCD_Visual_Demo(void)
{
	visualDemo();
}

#if COMPILE_TOUCH_FUNCTIONS == 1
void LCD_Touch_Polling_Demo(void)
{
	LCD_Clear(0,LCD_COLOR_GREEN);
	while (1) {
		/* If touch pressed */
		if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
			/* Touch valid */
			printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
			LCD_Clear(0, LCD_COLOR_RED);
		} else {
			/* Touch not pressed */
			printf("Not Pressed\n\n");
			LCD_Clear(0, LCD_COLOR_GREEN);
		}
	}
}
#endif // COMPILE_TOUCH_FUNCTIONS

//void LCD_SetUpBoard()
//{
//	// LCD_Driver command to set the background
//
//	// LCD_Driver commands to set the lines
//	// Set vertical lines
//	LCD_Draw_Vertical_Line(BOARD_XPosition0, BOARD_YPosition0, LCD_PIXEL_HEIGHT, LINE_COLOR0);
//	LCD_Draw_Vertical_Line(BOARD_XPosition1, BOARD_YPosition0, LCD_PIXEL_HEIGHT, LINE_COLOR0);
//	LCD_Draw_Vertical_Line(BOARD_XPosition2, BOARD_YPosition0, LCD_PIXEL_HEIGHT, LINE_COLOR0);
//	LCD_Draw_Vertical_Line(BOARD_XPosition3, BOARD_YPosition0, LCD_PIXEL_HEIGHT, LINE_COLOR0);
//	LCD_Draw_Vertical_Line(BOARD_XPosition4, BOARD_YPosition0, LCD_PIXEL_HEIGHT, LINE_COLOR0);
//	LCD_Draw_Vertical_Line(BOARD_XPosition5, BOARD_YPosition0, LCD_PIXEL_HEIGHT, LINE_COLOR0);
//	LCD_Draw_Vertical_Line(BOARD_XPosition6, BOARD_YPosition0, LCD_PIXEL_HEIGHT, LINE_COLOR0);
//	// Set horizontal lines
//	LCD_Draw_Horizontal_Line(BOARD_XPosition0, BOARD_yPosition0, LCD_PIXEL_WIDTH, LINE_COLOR0);
//	LCD_Draw_Horizontal_Line(BOARD_XPosition0, BOARD_yPosition1, LCD_PIXEL_WIDTH, LINE_COLOR0);
//	LCD_Draw_Horizontal_Line(BOARD_XPosition0, BOARD_yPosition2, LCD_PIXEL_WIDTH, LINE_COLOR0);
//	LCD_Draw_Horizontal_Line(BOARD_XPosition0, BOARD_yPosition3, LCD_PIXEL_WIDTH, LINE_COLOR0);
//	LCD_Draw_Horizontal_Line(BOARD_XPosition0, BOARD_yPosition4, LCD_PIXEL_WIDTH, LINE_COLOR0);
//	LCD_Draw_Horizontal_Line(BOARD_XPosition0, BOARD_yPosition5, LCD_PIXEL_WIDTH, LINE_COLOR0);
//
//}





