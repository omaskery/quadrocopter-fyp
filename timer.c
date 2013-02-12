#include "timer.h"

void TimerInitialise(timer *this, unsigned long _base, unsigned long _cycles)
{
	this->base = _base;
	this->cycles = _cycles;
	
	TimerConfigure(this->base, TIMER_CFG_PERIODIC);
	TimerLoadSet(this->base, TIMER_A, _cycles);
	TimerIntEnable(this->base, TIMER_TIMA_TIMEOUT);
}

void TimerStart(timer *this)
{
	TimerEnable(this->base, TIMER_A);
}
