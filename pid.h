#ifndef PID_H
#define PID_H

// structure to represent information needed for PID algorithm
typedef struct pid_t
{
	float current;					// current position
	float target;						// target position
	float last;							// last position
	float integral;					// accumulated integral
	float integralMax;			// maximum value for integral
	float integralMin;			// minimum value for integral
	float p, i, d;					// gain coefficients (Proportional, Integral and Differential)
	float output;						// the last calculated output from the PID algorithm
} pid;

// initialise a PID control loop
void PidInitialise(pid*, float, float, float);
// set the current target for a PID control loop
void PidSetTarget(pid*, float);
// set the current value for a PID control loop
void PidSetCurrent(pid*, float);
// cause a PID object to calculate a new output
void PidUpdate(pid*);

#endif
