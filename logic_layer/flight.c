#include "flight.h"
#include "../hardware_layer/motor.h"

flight flight_module;

void FlightInitialise(flight *this)
{
	this->thrust = 0.0f;
	this->desiredRoll = 0.0f;
	this->desiredPitch = 0.0f;
	this->desiredYaw = 0.0f;
	
#ifndef USE_ACCELERATION // PID for absolute angles
	PidInitialise(&this->roll,   0.002f, 0.001f, 0.001f);
	PidInitialise(&this->pitch,  0.000f, 0.000f, 0.000f);
  PidInitialise(&this->yaw,    0.000f, 0.000f, 0.000f);
#else // USE_ACCELERATION - PID for rotational rates (calculated: 147, 0.1, 0.025)
	PidInitialise(&this->roll,   0.300f, 0.150f, 0.100f);
	PidInitialise(&this->pitch,  0.300f, 0.150f, 0.100f);
  PidInitialise(&this->yaw,    0.000f, 0.000f, 0.000f);
	
	PidInitialise(&this->rollAngle,  0.006f, 0.002f, 0.002f);
	PidInitialise(&this->pitchAngle, 0.006f, 0.002f, 0.002f);
#endif // USE_ACCELERATION
	
	MotorSetSafetyClamps(0.050f, 0.600f);
	//MotorSetSafetyClamps(0.050f, 0.200f);
}

void FlightUpdate(flight *this, rotation *_rot)
{
#ifndef USE_ACCELERATION // feed absolute angles in
	PidSetCurrent(&this->roll,  _rot->angle.x);
	PidSetCurrent(&this->pitch, _rot->angle.y);
	PidSetCurrent(&this->yaw,   _rot->angle.z);
#else // USE_ACCELERATION - feed rotational rates (divided by sampling rate) in
	///*
	float deltaRoll, deltaPitch, deltaYaw;
	float deltaCoefficient = 0.01f;
	
	deltaRoll  = this->desiredRoll  - _rot->angle.x;
	deltaPitch = this->desiredPitch - _rot->angle.y;
	deltaYaw   = this->desiredYaw   - _rot->angle.z;
	
	// tell angle PIDs about desired rotations
	PidSetTarget(&this->rollAngle,  this->desiredRoll);
	PidSetTarget(&this->pitchAngle, this->desiredPitch);
	
	// tell the angle PIDs the current angle
	PidSetCurrent(&this->rollAngle,  _rot->angle.x);
	PidSetCurrent(&this->pitchAngle, _rot->angle.y);
	
	// allow angle PIDs to think
	PidUpdate(&this->rollAngle);
	PidUpdate(&this->pitchAngle);
	
	//PidSetTarget(&this->roll,   deltaRoll  * deltaCoefficient);
	//PidSetTarget(&this->pitch,  deltaPitch * deltaCoefficient);
	// use angle PIDs in setting the desired angular velocity on the roll/pitch PIDs
	PidSetTarget(&this->roll,   this->rollAngle.output);
	PidSetTarget(&this->pitch,  this->pitchAngle.output);
	PidSetTarget(&this->yaw,    deltaYaw   * deltaCoefficient);
	//*/
	
	// tell angular velocity PIDs the current angular velocities
	PidSetCurrent(&this->roll,  _rot->rate.x / 200.0f);
	PidSetCurrent(&this->pitch, _rot->rate.y / 200.0f);
	PidSetCurrent(&this->yaw,   _rot->rate.z / 200.0f);
#endif // USE_ACCELERATION
	
	// let angular velocity PIDs think
	PidUpdate(&this->roll);
	PidUpdate(&this->pitch);
	PidUpdate(&this->yaw);
	
	// use outputs from PIDs to set motor speeds
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
	this->desiredRoll = _roll;
	#ifndef USE_ACCELERATION
	PidSetTarget(&this->roll, _roll);
	#endif
}

void FlightSetDesiredPitch(flight *this, float _pitch)
{
	this->desiredPitch = _pitch;
	#ifndef USE_ACCELERATION
	PidSetTarget(&this->pitch, _pitch);
	#endif
}

void FlightSetDesiredYaw(flight *this, float _yaw)
{
	this->desiredYaw = _yaw;
	#ifndef USE_ACCELERATION
	PidSetTarget(&this->yaw, _yaw);
	#endif
}

void FlightSetThrust(flight *this, float _thrust)
{
	if(_thrust < 0.0f) _thrust = 0.0f;
	if(_thrust > 1.0f) _thrust = 1.0f;
	this->thrust = _thrust;
}
