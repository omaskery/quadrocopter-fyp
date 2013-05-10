#include "motor.h"
#include "../peripheral_drivers/usart.h"

#include "../includes.h"

motor motorA, motorB, motorC, motorD;
motor *all_motors[4];

float __globalMotorMin = 0.0f, __globalMotorMax = 1.0f;

void MotorInitialise(motor *this, pwm *_output)
{
	this->output = _output;
	this->enabled = 0;
	MotorSetPower(this, 0.0f);
	MotorOff(this);
}

void MotorSetPower(motor *this, float _power)
{
	// could've used if(!enabled) return, but this way prevents variables instantiating
	if(this->enabled)
	{
		unsigned long millisecond = SysCtlClockGet() / PwmClockDivider() / 1000;
		unsigned long minimum = millisecond * 1.1f;
		unsigned long maximum = millisecond * 2.1f;
		unsigned long range = maximum - minimum;
		
		if(_power < __globalMotorMin)
			_power = __globalMotorMin;
		else if(_power > __globalMotorMax)
			_power = __globalMotorMax;
		
		this->power = _power;
		this->cycles = minimum + (this->power * range);
		
		if(this->cycles > maximum)
			this->cycles = maximum;
		
		PwmSet(this->output, this->cycles);
	}
}

void MotorOff(motor *this)
{
	unsigned long millisecond = SysCtlClockGet() / PwmClockDivider() / 1000;
	unsigned long offCycles = millisecond * 0.9f;
	
	this->enabled = 0;
	PwmSet(this->output, offCycles);
}

void MotorsEnableAll(motor **_motors, int _count)
{
	int i;
	for(i = 0; i < _count; i++)
	{
		motor *m = _motors[i];
		m->enabled = 1;
		PwmSet(m->output, m->cycles);
	}
}

void MotorsDisableAll(motor **_motors, int _count)
{
	int i;
	for(i = 0; i < _count; i++)
	{
		motor *m = _motors[i];
		MotorOff(m);
	}
}

void MotorSetSafetyClamps(float _min, float _max)
{
	if(_min > _max) {
		float swap = _max;
		_max = _min;
		_min = swap;
	}
	
	if(_min < 0.0f) _min = 0.0f;
	if(_max > 1.0f) _max = 1.0f;
	
	__globalMotorMin = _min;
	__globalMotorMax = _max;
}
