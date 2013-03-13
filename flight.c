#include "flight.h"
#include "motor.h"

flight flight_module;

void FlightInitialise(flight *this)
{
	this->thrust = 0.0f;
#ifndef USE_ACCELERATION // PID for absolute angles
	PidInitialise(&this->roll,   0.002f, 0.001f, 0.001f);
	PidInitialise(&this->pitch,  0.000f, 0.000f, 0.000f);
  PidInitialise(&this->yaw,    0.000f, 0.000f, 0.000f);
#else // USE_ACCELERATION - PID for rotational rates
	PidInitialise(&this->roll,   1.000f, 0.400f, 0.100f);
	PidInitialise(&this->pitch,  0.000f, 0.000f, 0.000f);
  PidInitialise(&this->yaw,    0.000f, 0.000f, 0.000f);
#endif // USE_ACCELERATION
	MotorSetSafetyClamps(0.0f, 0.20f);
}

void FlightUpdate(flight *this, rotation *_rot)
{
#ifndef USE_ACCELERATION // feed absolute angles in
	PidSetCurrent(&this->roll,  _rot->angle.x);
	PidSetCurrent(&this->pitch, _rot->angle.y);
	PidSetCurrent(&this->yaw,   _rot->angle.z);
#else // USE_ACCELERATION - feed rotational rates (divided by sampling rate) in
	PidSetCurrent(&this->roll,  _rot->rate.x / 200.0f);
	PidSetCurrent(&this->pitch, _rot->rate.y / 200.0f);
	PidSetCurrent(&this->yaw,   _rot->rate.z / 200.0f);
#endif // USE_ACCELERATION
	
	PidUpdate(&this->roll);
	PidUpdate(&this->pitch);
	PidUpdate(&this->yaw);
	
	MotorSetPower(&motorA, this->thrust - this->pitch.output - this->yaw.output);
	MotorSetPower(&motorB, this->thrust - this->roll.output  + this->yaw.output);
	MotorSetPower(&motorC, this->thrust + this->pitch.output - this->yaw.output);
	MotorSetPower(&motorD, this->thrust + this->roll.output  + this->yaw.output);
}

void FlightSetDesiredRotation(flight *this, float _roll, float _pitch, float _yaw)
{
	FlightSetDesiredRoll(this, _roll);
	FlightSetDesiredPitch(this, _pitch);
	FlightSetDesiredYaw(this, _yaw);
}

void FlightSetDesiredRollPitch(flight *this, float _roll, float _pitch)
{
	FlightSetDesiredRoll(this, _roll);
	FlightSetDesiredPitch(this, _pitch);
}

void FlightSetDesiredRoll(flight *this, float _roll)
{
	PidSetTarget(&this->roll, _roll);
}

void FlightSetDesiredPitch(flight *this, float _pitch)
{
	PidSetTarget(&this->pitch, _pitch);
}

void FlightSetDesiredYaw(flight *this, float _yaw)
{
	PidSetTarget(&this->yaw, _yaw);
}

void FlightSetThrust(flight *this, float _thrust)
{
	if(_thrust < 0.0f) _thrust = 0.0f;
	if(_thrust > 1.0f) _thrust = 1.0f;
	this->thrust = _thrust;
}
