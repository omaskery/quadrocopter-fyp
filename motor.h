#ifndef MOTOR_H
#define MOTOR_H

#include "mypwm.h"

// represents a motor
typedef struct motor_t
{
	unsigned long cycles;			// the pulse width for the PWM output
	float power;							// the current motor power as a percent (0.0f -> 1.0f)
	pwm *output;							// the PWM output to write to
} motor;

// motors in this system
extern motor motorA, motorB, motorC, motorD;

// initialise the motor, takes this* and pointer to PWM object to use for output
void MotorInitialise(motor*, pwm*);
// set the motor power as a percentage, takes this* and float percentage power (0.0f -> 1.0f)
void MotorSetPower(motor*, float);

#endif
