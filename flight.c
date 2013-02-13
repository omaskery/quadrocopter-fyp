#include "flight.h"
#include "motor.h"

flight flight_module;

void FlightInitialise(flight *this)
{
	this->thrust = 0.0f;
	PidInitialise(&this->roll,   0.020f, 0.020f, 0.040f);
	PidInitialise(&this->pitch,  0.010f, 0.000f, 0.000f);
  PidInitialise(&this->yaw,    0.000f, 0.000f, 0.000f);
}

void FlightUpdate(flight *this, rotation *_rot)
{
	PidSetCurrent(&this->roll,  _rot->angle.x);
	PidSetCurrent(&this->pitch, _rot->angle.y);
	PidSetCurrent(&this->yaw,   _rot->angle.z);
	
	PidUpdate(&this->roll);
	PidUpdate(&this->pitch);
	PidUpdate(&this->yaw);
	
	MotorSetPower(&motorA, this->thrust + this->pitch.output - this->yaw.output);
	MotorSetPower(&motorB, this->thrust + this->roll.output  + this->yaw.output);
	MotorSetPower(&motorC, this->thrust - this->pitch.output - this->yaw.output);
	MotorSetPower(&motorD, this->thrust - this->roll.output  + this->yaw.output);
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
	this->thrust = _thrust;
}
