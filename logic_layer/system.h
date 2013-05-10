#ifndef SYSTEM_H
#define SYSTEM_H

#include "../peripheral_drivers/timer.h"

struct system_status_t;
// type definition for 'mode functions' that handle the current mode of operation of the device
typedef void (*mode_function)(struct system_status_t*);
// system status structure
typedef struct system_status_t
{
	uint32_t milliseconds;	// current system time in milliseconds
	mode_function mode;			// current 'mode' function to be run
	int modeChanged:1;			// flag indicating whether mode has just been switched to, requiring initialisation
	int killflag:1;					// flag indicating system should go into a safe state, forcing motors off
} system_status;

extern timer global_timer;
extern system_status sys;
extern timer sys_timer;

// initialise a system status object
void SystemInitialise(system_status*);
// set the mode function of a system status object (sets the modeChanged flag)
void SystemSetMode(system_status*, mode_function);
// calls the current mode function of a system to 'think'
void SystemThink(system_status*);
// disables the system by setting the kill flag
void SystemKillEnable(system_status*);
// re-enablse the system after the kill flag is set
void SystemKillDisable(system_status*);

#endif
