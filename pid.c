#include "pid.h"

void PidInitialise(pid *this, float _p, float _i, float _d)
{
	this->p = _p;
	this->i = _i;
	this->d = _d;
	this->target = 0.0f;
	this->current = 0.0f;
	this->last = 0.0f;
	this->output = 0.0f;
	this->integral = 0.0f;
	// arbitrary atm!
	this->integralMax = 1.0f;
	this->integralMin = -1.0f;
}

void PidSetTarget(pid *this, float _target)
{
	this->target = _target;
}

void PidSetCurrent(pid *this, float _current)
{
	this->current = _current;
}

void PidUpdate(pid *this)
{
	float error, delta;
	float pTerm, iTerm, dTerm;
	
	error = this->target - this->current;
	delta = this->current - this->last;
	this->integral += error;
	
	if(this->integral > this->integralMax)
		this->integral = this->integralMax;
	else if(this->integral < this->integralMin)
		this->integral = this->integralMin;
	
	pTerm = this->p * error;
	iTerm = this->i * this->integral;
	dTerm = this->d * delta;
	
	this->output = pTerm + iTerm + dTerm;
}
