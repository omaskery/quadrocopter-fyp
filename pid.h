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

void PidInitialise(pid*, float, float, float);
void PidSetTarget(pid*, float);
void PidSetCurrent(pid*, float);
void PidUpdate(pid*);

#endif
