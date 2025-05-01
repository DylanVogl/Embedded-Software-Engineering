/*
 * ApplicationCode.h
 *
 *  Created on: 04/27/2025
 *      Author: Dylan
 */

#include "LCD_Driver.h"
#include "stm32f4xx_hal.h"
#include "Scheduler.h"
#include "Button_Driver.h"
#include "Timer_Driver.h"
#include "Gyro_Driver.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#ifndef INC_APPLICATIONCODE_H_
#define INC_APPLICATIONCODE_H_

#define DROP_COLUMN_TOUCH_SELECTION 0

void ApplicationInit(void);

void buttonInit();

void timerInit();

void gyroInit();

int roundResetPrompt();

void roundReset();

void gameModeDisplay();

#if (COMPILE_TOUCH_FUNCTIONS == 1)
void connectFour(void);
#endif // (COMPILE_TOUCH_FUNCTIONS == 1)

int getCurrentPlayerColor(int player);

void switchPlayer();

int checkWinCondition();

void LCD_SetUpBoard();

void GameModeName(int gameMode);

void placeChip(int column);

int boardFull(void);

char* IntegerToAsciiArray(int input);

void displayWinScreen();

void opponentMove();

void singlePlayerMode();

void twoPlayerMode();




#endif /* INC_APPLICATIONCODE_H_ */
