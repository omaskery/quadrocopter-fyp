#ifndef FLIGHT_H
#define FLIGHT_H

#include "pid.h"
#include "motion.h"

typedef struct flight_t
{
	float desiredRoll, desiredPitch, desiredYaw;
	pid roll, pitch, yaw;
	float thrust;
} flight;

extern flight flight_module;

// initialise flight module
void FlightInitialise(flight *this);
// update flight module
void FlightUpdate(flight *this, rotation *_rot);
// set specific target conditions
void FlightSetDesiredRotation(flight *this, float _roll, float _pitch, float _yaw);
void FlightSetDesiredRollPitch(flight *this, float _roll, float _pitch);
void FlightSetDesiredRoll(flight *this, float _roll);
void FlightSetDesiredPitch(flight *this, float _pitch);
void FlightSetDesiredYaw(flight *this, float _yaw);
void FlightSetThrust(flight *this, float _thrust);

#endif
