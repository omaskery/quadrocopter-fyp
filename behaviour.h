#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include "system.h"

// initial warmup period (gives motors/ESCs time to initialise)
void _Mode_Warmup(system_status*);
// mode for performing main operation
void _Mode_Running(system_status*);
// mode for doing minimal work during emergency shutdown operation
void _Mode_Dead(system_status*);

// mode for configuring motors on startup
void _Mode_MotorConfigure(system_status*);

#endif
