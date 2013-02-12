#include "system.h"
#include "behaviour.h"
#include "motor.h"
#include "globals.h"
#include "usart.h"

system_status sys;
timer sys_timer;

void SystemInitialise(system_status *this)
{
#ifdef MOTOR_CONFIG
	const int configureMotors = 1;
#else
	const int configureMotors = 0;
#endif
	
	this->killflag = 0;
	this->modeChanged = 0;
	
	if(!configureMotors)
	{
		SystemSetMode(this, _Mode_Warmup);
	}
	else
	{
		SystemSetMode(this, _Mode_MotorConfigure);
	}
}

void SystemSetMode(system_status *this, mode_function _mode)
{
	this->mode = _mode;
	this->modeChanged = 1;
}

void SystemThink(system_status *this)
{
	mode_function m = this->mode;
	this->mode(this);
	if(m == this->mode)
		this->modeChanged = 0;
}

void SystemKillEnable(system_status *this)
{
	this->killflag = 1;
	MotorsDisableAll(all_motors, 4);
	SystemSetMode(this, _Mode_Dead);
}

void SystemKillDisable(system_status *this)
{
	this->killflag = 0;
}
