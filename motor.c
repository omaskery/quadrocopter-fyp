#include "motor.h"
#include "usart.h"

#include "includes.h"

motor motorA, motorB, motorC, motorD;

void MotorInitialise(motor *this, pwm *_output)
{
	this->output = _output;
	MotorSetPower(this, 0.0f);
}

void MotorSetPower(motor *this, float _power)
{
	unsigned long minimum = SysCtlClockGet() / PwmClockDivider() / 1000;
	unsigned long maximum = minimum * 2;
	unsigned long range = minimum;
	
	if(_power < 0.0f)
		_power = 0.0f;
	else if(_power > 1.0f)
		_power = 1.0f;
	
	this->power = _power;
	this->cycles = minimum + (this->power * range);
	
	if(this->cycles > maximum)
		this->cycles = maximum;
	
	PwmSet(this->output, this->cycles);
}
