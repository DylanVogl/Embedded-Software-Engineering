/*
 * Timer_Driver.c
 *
 *  Created on: Apr 26, 2025
 *      Author: dvogl
 */

#include "Timer_Driver.h"

void Timer_Init()
{
	__HAL_RCC_TIM2_CLK_ENABLE();  // Enable TIM2 clock

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = PRESCALER;  // Adjust for 1 ms ticks (if timer clock is 80 MHz)
	htim2.Init.Period = 0xFFFF;   // Max value for 16-bit timer
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	// Initialize the timer
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
	    // Handle initialization error
	    printf("Timer initialization failed.\n");
	    while (1);
	}
}

void Timer_Start()
{
	HAL_TIM_Base_Start(&htim2);
}

void Timer_Stop()
{
	HAL_TIM_Base_Stop(&htim2);
}

void Timer_Reset()
{
	__HAL_TIM_SET_COUNTER(&htim2, 0);
}

int Timer_getTime()
{
	uint32_t timeValueRecorded = __HAL_TIM_GET_COUNTER(&htim2);
	return timeValueRecorded;
}

int Timer_ElapsedTime(uint32_t startTime, uint32_t endTime)
{
	uint32_t elapsedTime = (endTime >= startTime)
                           ? (endTime - startTime)
                           : ((PERIOD - startTime) + endTime + 1);
    return (elapsedTime + (TICK_FREQUENCY)) / (2*TICK_FREQUENCY);  // Rounded seconds
}



