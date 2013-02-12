#ifndef TIMER_H
#define TIMER_H

#include "includes.h"

// struct for timer peripherals
typedef struct timer_t
{
	unsigned long cycles;			// the number of cycles this timer counts to before 'triggering'
	unsigned long base;				// the base address of this timer peripheral
} timer;

// initialise the specified timer, taking a 'this', base address and number of cycles
void TimerInitialise(timer*, unsigned long, unsigned long);
// starts a timer
void TimerStart(timer*);

#endif
