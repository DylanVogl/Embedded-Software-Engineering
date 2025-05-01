/*
 * Scheduler.h
 *
 *  Created on: Feb 11, 2025
 *      Author: dvogl
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "stm32f4xx_hal.h"

// Create a bit mask to access the 0th bit.
#define APP_DELAY_FLAG_EVENT (1 << 0)

// Create a bit mask to access the 1st bit.
#define DROP_CHIP_EVENT (1 << 1)

// a. Brief: This will return the scheduled events.
// b. Return Arguments:
// i. A uint32_t that will be the scheduled events.
int getScheduledEvents();

// a. Brief: Adds an event to be scheduled.
// b. Input Arguments:
// i. A uint32_t value that will be the event to be scheduled.
void addSchedulerEvent(uint32_t eventScheduled);

// a. Brief: Removes event to be scheduled.
// b. Input Arguments:
// i. A uint32_t value that will be an event to be removed.
void removeSchedulerEvent(uint32_t eventRemoved);




#endif /* SCHEDULER_H_ */

