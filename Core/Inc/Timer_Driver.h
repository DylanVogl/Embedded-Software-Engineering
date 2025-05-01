/*
 * Timer_Driver.h
 *
 *  Created on: Apr 26, 2025
 *      Author: dvogl
 */

#ifndef INC_TIMER_DRIVER_H_
#define INC_TIMER_DRIVER_H_

#include "stm32f4xx_hal.h"
#include <stdio.h>
extern TIM_HandleTypeDef htim2;

#define PERIOD 0xFFFF        // Auto-reload value or maximum timer count
#define PRESCALER ((HAL_RCC_GetPCLK1Freq() / 1000) - 1)
#define TICK_FREQUENCY 1000

void Timer_Init();

void Timer_Start();

void Timer_Stop();

void Timer_Reset();

int Timer_getTime();

int Timer_ElapsedTime(uint32_t startTime, uint32_t endTime);

#endif /* INC_TIMER_DRIVER_H_ */
