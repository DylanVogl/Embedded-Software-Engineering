/*
 * Scheduler.c
 *
 *  Created on: Feb 11, 2025
 *      Author: dvogl
 */

#include "Scheduler.h"

static uint32_t scheduledEvents;

// a. Brief: This will return the scheduled events.
// b. Return Arguments:
// i. A uint32_t that will be the scheduled events.
int getScheduledEvents()
{
	return scheduledEvents;
}

// a. Brief: Adds an event to be scheduled.
// b. Input Arguments:
// i. A uint32_t value that will be the event to be scheduled.
/*
 * In the function to add a scheduled event:
 * a. Given the input argument, that should be a valid bit of a 32-bit value.
 * Make sure to set that bit the same way you would set a bit in a register.
 */
void addSchedulerEvent(uint32_t eventScheduled)
{
	scheduledEvents |= (eventScheduled);
}

// a. Brief: Removes event to be scheduled.
// b. Input Arguments:
// i. A uint32_t value that will be an event to be removed.
/*
 * In the function to remove an event:
 * a. Given the input argument, that should be a valid bit of a 32-bit value.
 * Make sure to clear that bit the same way you would set a bit in a register.
 */
void removeSchedulerEvent(uint32_t eventRemoved)
{
	scheduledEvents &= ~(eventRemoved);
}

