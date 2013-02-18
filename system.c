#include "system.h"
#include "behaviour.h"
#include "motor.h"
#include "globals.h"
#include "usart.h"

timer global_timer;
system_status sys;
timer sys_timer;

void SystemInitialise(system_status *this)
{
	this->killflag = 0;
	this->modeChanged = 0;

#ifdef MOTOR_CONFIG		// check for motor config mode
	SystemSetMode(this, _Mode_MotorConfigure);
#else
#ifdef TEST_MODE			// check for test mode
	SystemSetMode(this, _Mode_TestMode);
#else									// otherwise normal operation
	SystemSetMode(this, _Mode_Warmup);
#endif
#endif
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
