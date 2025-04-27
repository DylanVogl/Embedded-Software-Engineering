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
    buttonInit();
    timerInit();
    LCD_Clear(0,LCD_COLOR_WHITE);

    #if COMPILE_TOUCH_FUNCTIONS == 1
	InitializeLCDTouch();
	HAL_Delay(500);

	// This is the orientation for the board to be directlty up where the buttons are vertically above the screen
	// Top left would be low x value, high y value. Bottom right would be low x value, low y value.
	StaticTouchData.orientation = STMPE811_Orientation_Portrait_1;

	#endif // COMPILE_TOUCH_FUNCTIONS
}

void buttonInit()
{
    Button_Init();  // Call the button initialization function
}


void timerInit()
{
	Timer_Init();
}

void LCD_Visual_Demo(void)
{
	visualDemo();
}

int gameEnd = 0;
int tie = 0;
int dropButtonPressed = 0;
int currentPlayer = 1;
int dropColumn = 3;
int gameBoard[6][7] = {0};
int gameMode = 0;
int playerOneScore = 0;
int playerTwoScore = 0;
uint32_t startTime = 0;
uint32_t endTime = 0;

int roundResetPrompt()
{
	// Reset Button Display
	LCD_Draw_Vertical_Line(CENTER_BUTTON_BOUND_X1, TWOPLAYER_BUTTON_BOUND_Y1, GAMEMODE_BUTTON_LENGTH, LCD_COLOR_MAGENTA);
	LCD_Draw_Vertical_Line(CENTER_BUTTON_BOUND_X2, TWOPLAYER_BUTTON_BOUND_Y1, GAMEMODE_BUTTON_LENGTH, LCD_COLOR_MAGENTA);
	LCD_Draw_Horizontal_Line(CENTER_BUTTON_BOUND_X1, TWOPLAYER_BUTTON_BOUND_Y1, GAMEMODE_BUTTON_WIDTH, LCD_COLOR_MAGENTA);
	LCD_Draw_Horizontal_Line(CENTER_BUTTON_BOUND_X1, TWOPLAYER_BUTTON_BOUND_Y2, GAMEMODE_BUTTON_WIDTH, LCD_COLOR_MAGENTA);
	LCD_DisplayChar(95,185,'N');
	LCD_DisplayChar(110,185,'e');
	LCD_DisplayChar(125,185,'w');

	LCD_DisplayChar(85,215,'R');
	LCD_DisplayChar(100,215,'o');
	LCD_DisplayChar(115,215,'u');
	LCD_DisplayChar(130,215,'n');
	LCD_DisplayChar(145,215,'d');

	while (1)
	{
		// Retrieve touch data and update coordinates
	    if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed)
	    {
	    	printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);

	        // Check if the touch is within the another round region
	        if ((StaticTouchData.x >= CENTER_BUTTON_BOUND_X1) && (StaticTouchData.x <= CENTER_BUTTON_BOUND_X2) &&
	            (StaticTouchData.y >= TWOPLAYER_BUTTON_BOUND_Y1) && (StaticTouchData.y <= TWOPLAYER_BUTTON_BOUND_Y2))
	        {
	        	return 1;
	        }
	    }
	}
	return 0;
}

void roundReset()
{
	dropButtonPressed = 0;
	currentPlayer = 1;
	dropColumn = 3;
	memset(gameBoard, 0, sizeof(gameBoard));
	gameMode = 0;
	gameEnd = 0;
	tie = 0;
	startTime = 0;
	endTime = 0;
	Timer_Reset();
}

#if COMPILE_TOUCH_FUNCTIONS == 1
void LCD_Touch_Polling_Demo(void) {

    visualDemo();
    while (1)
    {
        // Retrieve touch data and update coordinates
        if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed)
        {
            printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);

            // Check if the touch is within the single-player region
            if ((StaticTouchData.x >= CENTER_BUTTON_BOUND_X1) && (StaticTouchData.x <= CENTER_BUTTON_BOUND_X2) &&
                (StaticTouchData.y >= SINGLEPLAYER_BUTTON_BOUND_Y1) && (StaticTouchData.y <= SINGLEPLAYER_BUTTON_BOUND_Y2))
            {

                // Enter Single player mode
                singlePlayerMode();
            }

            // Check if the touch is within the two-player region
            else if ((StaticTouchData.x >= CENTER_BUTTON_BOUND_X1) && (StaticTouchData.x <= CENTER_BUTTON_BOUND_X2) &&
                     (StaticTouchData.y >= TWOPLAYER_BUTTON_BOUND_Y1) && (StaticTouchData.y <= TWOPLAYER_BUTTON_BOUND_Y2))
            {
                // Enter Two player mode
                twoPlayerMode();
            }
        }
        if(gameEnd)
        {
        	// If the start another round button is pressed reset values and call the function that was first pressed
        	if(roundResetPrompt())
        	{
        		if(gameMode)
        		{
        			// Reset values including gameEnd
        			roundReset();
        			twoPlayerMode();
        		}
        		else
        		{
        			// Reset values including gameEnd
        			roundReset();
        			singlePlayerMode();
        		}
        	}
        	else
        	{
        		break;
        	}
        }
    }
}
#endif // COMPILE_TOUCH_FUNCTIONS

int getCurrentPlayerColor(int player)
{
	if(player == 1)
	{
		return LCD_COLOR_RED;
	}
	if(player == 2)
	{
		return LCD_COLOR_YELLOW;
	}
	return LCD_COLOR_GREEN;
}

void switchPlayer()
{
    currentPlayer = (currentPlayer == 1) ? 2 : 1; // Toggle between Player 1 and Player 2
}

int checkWinCondition()
{
    // Check horizontal, vertical, and diagonal patterns for a win
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLUMNS; col++)
        {
            int player = gameBoard[row][col];
            if (player == 0) continue;

            // Check horizontal win
            if (col <= COLUMNS - 4 &&
                player == gameBoard[row][col + 1] &&
                player == gameBoard[row][col + 2] &&
                player == gameBoard[row][col + 3])
                return 1;

            // Check vertical win
            if (row <= ROWS - 4 &&
                player == gameBoard[row + 1][col] &&
                player == gameBoard[row + 2][col] &&
                player == gameBoard[row + 3][col])
                return 1;

            // Check diagonal (bottom-right)
            if (row <= ROWS - 4 && col <= COLUMNS - 4 &&
                player == gameBoard[row + 1][col + 1] &&
                player == gameBoard[row + 2][col + 2] &&
                player == gameBoard[row + 3][col + 3])
                return 1;

            // Check diagonal (bottom-left)
            if (row <= ROWS - 4 && col >= 3 &&
                player == gameBoard[row + 1][col - 1] &&
                player == gameBoard[row + 2][col - 2] &&
                player == gameBoard[row + 3][col - 3])
                return 1;
        }
    }
    return 0; // No win detected
}

void LCD_SetUpBoard()
{
	// LCD_Driver commands to set the lines
	// Set vertical lines
	for(int x = 1; x < COLUMNS; x++)
	{
		LCD_Draw_Vertical_Line((COLUMN_WIDTH * x), BOARD_TOPLINE, CONNECT4_BOARD_HEIGHT, LCD_COLOR_BLACK);
	}
	// Set horizontal lines
	for(int y = 0; y < ROWS; y++)
	{
		LCD_Draw_Horizontal_Line(0, ((ROW_HEIGHT * y) + BOARD_TOPLINE), CONNECT4_BOARD_WIDTH, LCD_COLOR_BLACK);
	}
}

void GameModeName(int gameMode)
{
	LCD_SetFont(&Font12x12);
	if(gameMode)
	{
		LCD_DisplayChar(115,5,'T');
		LCD_DisplayChar(130,5,'w');
		LCD_DisplayChar(145,5,'o');
	}
	else
	{
		LCD_DisplayChar(70,5,'S');
		LCD_DisplayChar(85,5,'i');
		LCD_DisplayChar(100,5,'n');
		LCD_DisplayChar(115,5,'g');
		LCD_DisplayChar(130,5,'l');
		LCD_DisplayChar(145,5,'e');
	}

	LCD_DisplayChar(160,5,'P');
	LCD_DisplayChar(175,5,'l');
	LCD_DisplayChar(190,5,'a');
	LCD_DisplayChar(205,5,'y');
	LCD_DisplayChar(220,5,'e');
	LCD_DisplayChar(235,5,'r');
}

void displayUpdatedChipsAndBoard()
{
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLUMNS; col++)
	    {
			int player = gameBoard[row][col];
	        if (player == 0) continue;
            // Check and print for each color of chip
            if (gameBoard[row][col] == 1)
            {
            	LCD_Draw_Circle_Fill(((col*COLUMN_WIDTH) - CHIP_OFFSET), ((row*ROW_HEIGHT) - CHIP_OFFSET + BOARD_TOPLINE), CHIP_RADIUS, LCD_COLOR_RED);
            }
            if(gameBoard[row][col] == 2)
            {
            	LCD_Draw_Circle_Fill(((col*COLUMN_WIDTH) - CHIP_OFFSET), ((row*ROW_HEIGHT) - CHIP_OFFSET + BOARD_TOPLINE), CHIP_RADIUS, LCD_COLOR_YELLOW);
            }
	    }
	}
}

void placeChip(int column)
{
    for (int row = ROWS - 1; row >= 0; row--)
    {
    	// Check for an empty space
    	if (gameBoard[row][column] == 0)
        {
            gameBoard[row][column] = currentPlayer; // Place the chip
            LCD_Draw_Circle_Fill(((column * COLUMN_WIDTH) + CHIP_OFFSET), ((row * ROW_HEIGHT) + CHIP_OFFSET + BOARD_TOPLINE), CHIP_RADIUS, getCurrentPlayerColor(currentPlayer));
            break;
        }
    }
}

int boardFull(void)
{
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLUMNS; col++)
	    {
	        // Check and print for each color of chip
	        if (gameBoard[row][col] == 0)
	        {
	          	return 0;
	        }
	    }
	}
	return 1;
}

// Function to convert an integer to an ASCII array
char* IntegerToAsciiArray(int input) {
    // Validate input
    if (input < 0) {
        printf("Error: Negative input value.\n");
        return NULL;
    }

    // Handle zero case
    if (input == 0) {
        char* asciiArray = (char*)malloc(2 * sizeof(char));  // Space for '0' and '\0'
        if (asciiArray == NULL) {
            printf("Memory allocation failed!\n");
            return NULL;
        }
        asciiArray[0] = '0';
        asciiArray[1] = '\0';
        return asciiArray;
    }

    // Determine number of digits
    int temp = input, numDigits = 0;
    while (temp != 0) {
        numDigits++;
        temp /= 10;
    }

    // Allocate memory for ASCII array
    char* asciiArray = (char*)malloc((numDigits + 1) * sizeof(char));
    if (asciiArray == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    // Convert digits to ASCII
    asciiArray[numDigits] = '\0';  // Null-terminate
    for (int i = numDigits - 1; i >= 0; i--) {
        asciiArray[i] = '0' + (input % 10);
        input /= 10;
    }

    return asciiArray;
}

void displayEndScreen()
{
	gameEnd = 1;    // Set game end flag to true
	char* ASCII_array;  // Use char* for dynamic allocation

	LCD_Clear(0, LCD_COLOR_GREEN);
	LCD_SetFont(&Font16x24);

	if(!tie)
	{
		//Print out text to the screen displaying time winning player
		LCD_DisplayChar(65,60,'W');
		LCD_DisplayChar(80,60,'i');
		LCD_DisplayChar(95,60,'n');
		LCD_DisplayChar(110,60,'n');
		LCD_DisplayChar(125,60,'e');
		LCD_DisplayChar(140,60,'r');
		LCD_DisplayChar(155,60,':');

		LCD_DisplayChar(35,85,'P');
		LCD_DisplayChar(50,85,'l');
		LCD_DisplayChar(65,85,'a');
		LCD_DisplayChar(80,85,'y');
		LCD_DisplayChar(95,85,'e');
		LCD_DisplayChar(110,85,'r');

		if(currentPlayer == 1)
		{
			LCD_DisplayChar(140,85,'O');
			LCD_DisplayChar(155,85,'n');
			LCD_DisplayChar(170,85,'e');
			playerOneScore++;
		}
		else if(currentPlayer == 2)
		{
			LCD_DisplayChar(140,85,'T');
			LCD_DisplayChar(155,85,'w');
			LCD_DisplayChar(170,85,'o');
			playerTwoScore++;
		}

		// Player 1's Score
		ASCII_array = IntegerToAsciiArray(playerOneScore);
		LCD_DisplayChar(35,120,'P');
		LCD_DisplayChar(50,120,'1');
		LCD_DisplayChar(65,120,':');
		// Loop to display multi-digit score
		for (int i = 0; i < strlen(ASCII_array); i++)
		{
		    LCD_DisplayChar((80 + (15 * i)), 120, ASCII_array[i]);
		}
		free(ASCII_array);  // Free allocated memory

		// Player 2's Score
		ASCII_array = IntegerToAsciiArray(playerTwoScore);
		LCD_DisplayChar(135,120,'P');
		LCD_DisplayChar(150,120,'2');
		LCD_DisplayChar(165,120,':');
		// Loop to display multi-digit score
		for (int i = 0; i < strlen(ASCII_array); i++)
		{
		    LCD_DisplayChar((180 + (15 * i)), 120, ASCII_array[i]);
		}
		free(ASCII_array);  // Free allocated memory
	}
	else
	{
		LCD_DisplayChar(95,85,'T');
		LCD_DisplayChar(110,85,'i');
		LCD_DisplayChar(125,85,'e');
		LCD_DisplayChar(140,85,'!');
	}


	// Time of Round
	LCD_DisplayChar(50,150,'T');
	LCD_DisplayChar(65,150,'i');
	LCD_DisplayChar(80,150,'m');
	LCD_DisplayChar(95,150,'e');
	LCD_DisplayChar(110,150,':');
	// Convert elapsed time to ASCII array
	ASCII_array = IntegerToAsciiArray(Timer_ElapsedTime(startTime, endTime));
	for (int i = 0; i < strlen(ASCII_array); i++)
	{
	    LCD_DisplayChar((125 + (15 * i)), 150, ASCII_array[i]);
	}
	free(ASCII_array);  // Free allocated memory

	LCD_DisplayChar(170,150,'s');  // Display an s for seconds
}

int isColumnFull(int col)
{
    return gameBoard[0][col] != 0;  // Top row occupied means the column is full
}
int findAvailableRow(int col)
{
    for (int row = ROWS - 1; row >= 0; row--)
    {
        if (gameBoard[row][col] == 0) return row;
    }
    return -1;  // Column is full
}

int evaluatePattern(int row, int col, int deltaRow, int deltaCol)
{
    int aiCount = 0;
    int opponentCount = 0;

    // Iterate through the 4-slot pattern
    for (int i = 0; i < 4; i++)
    {
        int cell = gameBoard[row + i * deltaRow][col + i * deltaCol];

        if (cell == 2)  // AI chip
        {
            aiCount++;
        }
        else if (cell == 1)  // Opponent chip
        {
            opponentCount++;
        }
    }

    // Assign scores based on pattern analysis
    if (aiCount > 0 && opponentCount == 0)  // Favorable pattern for AI
    {
        switch (aiCount)
        {
            case 1: return 10;  // One AI chip
            case 2: return 50;  // Two AI chips
            case 3: return 100; // Three AI chips
            case 4: return 1000; // Four AI chips (win condition)
        }
    }
    else if (opponentCount > 0 && aiCount == 0)  // Threatening pattern for opponent
    {
        switch (opponentCount)
        {
            case 1: return -10;  // One opponent chip
            case 2: return -50;  // Two opponent chips
            case 3: return -100; // Three opponent chips (block required)
            case 4: return -1000; // Four opponent chips (lose condition)
        }
    }

    return 0;  // Neutral pattern
}

int evaluateBoard(void)
{
    int score = 0;

    // Evaluate rows for potential wins
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLUMNS - 3; col++)  // Start of a 4-slot
        {
            score += evaluatePattern(row, col, 0, 1);  // Check horizontal pattern
        }
    }

    // Evaluate columns for potential wins
    for (int col = 0; col < COLUMNS; col++)
    {
        for (int row = 0; row < ROWS - 3; row++)  // Start of a 4-slot
        {
            score += evaluatePattern(row, col, 1, 0);  // Check vertical pattern
        }
    }

    // Evaluate diagonals (bottom-left to top-right)
    for (int row = 0; row < ROWS - 3; row++)
    {
        for (int col = 0; col < COLUMNS - 3; col++)  // Start of diagonal
        {
            score += evaluatePattern(row, col, 1, 1);  // Check diagonal pattern
        }
    }

    // Evaluate diagonals (top-left to bottom-right)
    for (int row = 3; row < ROWS; row++)
    {
        for (int col = 0; col < COLUMNS - 3; col++)  // Start of diagonal
        {
            score += evaluatePattern(row, col, -1, 1);  // Check anti-diagonal pattern
        }
    }

    return score;
}

void opponentMove()
{
    int bestColumn = -1;
    int maxScore = -1;

    // Evaluate each column
    for (int col = 0; col < COLUMNS; col++)
    {
        // Skip full columns
        if (isColumnFull(col)) continue;

        // Simulate placing a chip in the column
        int row = findAvailableRow(col);
        gameBoard[row][col] = 2;  // The "2" represents the AI's chip

        // Evaluate the board state after the move
        int score = evaluateBoard();

        // Undo the move
        gameBoard[row][col] = 0;

        // Keep track of the best move
        if (score > maxScore)
        {
            maxScore = score;
            bestColumn = col;
        }
    }

    // Place the chip in the best column
    if (bestColumn != -1)
    {
        placeChip(bestColumn);
    }
}

void singlePlayerMode()
{
	Timer_Start();
	startTime = Timer_getTime();
	int currentX = 120;
	LCD_Clear(0, LCD_COLOR_GREEN);
	LCD_SetUpBoard();
	GameModeName(gameMode);
	while(1)
	{
		// Start timer
		if(currentPlayer == 2)
		{
			opponentMove();
			//displayUpdatedChipsAndBoard();
		}
		else
		{
			dropColumn = 3;
			currentX = 120;
			LCD_Draw_Circle_Fill(currentX, CHIP_PLACEMENT_ROW, CHIP_RADIUS, getCurrentPlayerColor(currentPlayer));
		}
		while(!dropButtonPressed && currentPlayer == 1)
		{
			if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed)
			{
				printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
				// Check if the touch is within the move left button region
				if ((StaticTouchData.x <= MOVE_LEFT_BUTTON_BOUND) && (dropColumn > 0))
				{
					// Move Chip Left
					// Erase current chip
					LCD_Draw_Circle_Fill(currentX, CHIP_PLACEMENT_ROW, CHIP_RADIUS, BACKGROUND_COLOR);
					// Move chip left
					currentX -= COLUMN_WIDTH;
					dropColumn -= 1;
					HAL_Delay(250);
					// Draw new chip
					LCD_Draw_Circle_Fill(currentX, CHIP_PLACEMENT_ROW, CHIP_RADIUS, getCurrentPlayerColor(currentPlayer));
				}
				// Check if the touch is within the move right button region
				else if ((StaticTouchData.x >= MOVE_RIGHT_BUTTON_BOUND) && (dropColumn < 6))
				{
					// Move Chip Right
					// Erase current chip
					LCD_Draw_Circle_Fill(currentX, CHIP_PLACEMENT_ROW, CHIP_RADIUS, BACKGROUND_COLOR);
					// Move chip left
					currentX += COLUMN_WIDTH;
					dropColumn += 1;
					HAL_Delay(250);
					// Draw new chip
					LCD_Draw_Circle_Fill(currentX, CHIP_PLACEMENT_ROW, CHIP_RADIUS, getCurrentPlayerColor(currentPlayer));
				}
			}
		}
		if (checkWinCondition())
		{
			Timer_Stop();
			endTime = Timer_getTime();
		    printf("Player %d Wins!\n", currentPlayer);
		    displayEndScreen();
		    break; // Exit loop when a winner is found
		}
		if(boardFull())
		{
			Timer_Stop();
			endTime = Timer_getTime();
			tie = 1;
			displayEndScreen();
			break;
		}
		switchPlayer(); // Switch players
		LCD_Draw_Circle_Fill(currentX, CHIP_PLACEMENT_ROW, CHIP_RADIUS, BACKGROUND_COLOR);
		dropButtonPressed = 0;
		}
}
void twoPlayerMode()
{
	Timer_Start();
	startTime = Timer_getTime();
	int currentX = 120;
	LCD_Clear(0, LCD_COLOR_GREEN);
	LCD_SetUpBoard();
	gameMode = 1;
	GameModeName(gameMode);
	while(1)
	{
		// Start timer
		dropColumn = 3;
		currentX = 120;
		LCD_Draw_Circle_Fill(currentX, CHIP_PLACEMENT_ROW, CHIP_RADIUS, getCurrentPlayerColor(currentPlayer));

		while(!dropButtonPressed)
		{
			if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed)
			{
				printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
				// Check if the touch is within the move left button region
				if ((StaticTouchData.x <= MOVE_LEFT_BUTTON_BOUND) && (dropColumn > 0))
				{
					// Move Chip Left
					// Erase current chip
					LCD_Draw_Circle_Fill(currentX, CHIP_PLACEMENT_ROW, CHIP_RADIUS, BACKGROUND_COLOR);
					// Move chip left
					currentX -= COLUMN_WIDTH;
					dropColumn -= 1;
					HAL_Delay(250);
					// Draw new chip
					LCD_Draw_Circle_Fill(currentX, CHIP_PLACEMENT_ROW, CHIP_RADIUS, getCurrentPlayerColor(currentPlayer));
				}
				// Check if the touch is within the move right button region
				else if ((StaticTouchData.x >= MOVE_RIGHT_BUTTON_BOUND) && (dropColumn < 6))
				{
					// Move Chip Right
					// Erase current chip
					LCD_Draw_Circle_Fill(currentX, CHIP_PLACEMENT_ROW, CHIP_RADIUS, BACKGROUND_COLOR);
					// Move chip left
					currentX += COLUMN_WIDTH;
					dropColumn += 1;
					HAL_Delay(250);
					// Draw new chip
					LCD_Draw_Circle_Fill(currentX, CHIP_PLACEMENT_ROW, CHIP_RADIUS, getCurrentPlayerColor(currentPlayer));
				}
			}
		}
		if (checkWinCondition())
		{
			Timer_Stop();
			endTime = Timer_getTime();
		    printf("Player %d Wins!\n", currentPlayer);
		    displayEndScreen();

		    break; // Exit loop when a winner is found
		}
		if(boardFull())
		{
			Timer_Stop();
			endTime = Timer_getTime();
			tie = 1;
			displayEndScreen();
			break;
		}
		LCD_Draw_Circle_Fill(currentX, CHIP_PLACEMENT_ROW, CHIP_RADIUS, BACKGROUND_COLOR);
		switchPlayer(); // Switch players
		dropButtonPressed = 0;

	}

}

int EXTI0_IRQHandler()
{
	// Disable EXTI0 interrupt
	HAL_NVIC_DisableIRQ(EXTI0_IRQn);


	//addSchedulerEvent(DROP_CHIP_EVENT);

	placeChip(dropColumn);

	//displayUpdatedChipsAndBoard();

	dropButtonPressed = 1;

	// Clear the pending EXTI interrupt for the button pin
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);

	// Re-enable EXTI0 interrupt
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);


	return 0;
}
