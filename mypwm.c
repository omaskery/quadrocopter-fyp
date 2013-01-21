#include "mypwm.h"
#include "usart.h"

pwm_generator pwm_gen0, pwm_gen1;
pwm pwm0, pwm1, pwm2, pwm3;

pwm_generator *all_pwm_gens[2];
pwm *all_pwm[4];

const int pwmDivider = 16;

int  PwmClockDivider(void)
{
	return pwmDivider;
}

void PwmGeneratorInitialise(pwm_generator *this, unsigned long _base, unsigned long _generator, unsigned long _msPeriod)
{
	unsigned long periodClocks = ((SysCtlClockGet() / pwmDivider) / 1000) * _msPeriod;
	
	this->number = _generator;
	this->period = periodClocks;
	this->base = _base;
	
	PWMGenConfigure(this->base, this->number, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC | PWM_GEN_MODE_DBG_RUN);
	PWMGenPeriodSet(this->base, this->number, periodClocks - 1);
}

void PwmInitialise(pwm *this, pwm_generator *_generator, unsigned long _number, unsigned long _width)
{
	this->generator = _generator;
	this->number = _number;
	
	PwmSet(this, _width);
}

void PwmSet(pwm *this, unsigned long _width)
{
	PWMPulseWidthSet(this->generator->base, this->number, _width);
}

void PwmSetPercentage(pwm *this, float _percent)
{
	unsigned long amount = this->generator->period * _percent;
	PwmSet(this, amount);
}

void PwmGeneratorEnable(const pwm_generator *this)
{
	PWMGenEnable(this->base, this->number);
}

void PwmGeneratorDisable(const pwm_generator *this)
{
	PWMGenDisable(this->base, this->number);
}
