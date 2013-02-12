
#include "includes.h"
#include "usart.h"
#include "motor.h"
#include "globals.h"
#include "system.h"
#include "iic.h"
#include "init.h"

void system_think(void)
{
	IntMasterDisable();
	
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	
	SystemThink(&sys);
	
	IntMasterEnable();
}

int main(void)
{
	InitialiseSystem();
	
	while(1)
	{
		if(UsartIsDataWaiting(&usart0))
		{
			char c = UsartGet(&usart0);
			if(c == 'k')
			{
				SystemKillEnable(&sys);
				UsartWriteLine(&usart0, "Killflag engaged");
			}
			else if(c == 'K' && sys.killflag)
			{
				SystemKillDisable(&sys);
				UsartWriteLine(&usart0, "Killflag disengaged");
			}
	#ifdef MOTOR_CONFIG
			else if(c == 'M')
			{
				unsigned long i;
				UsartWriteLine(&usart0, "Motors at 100%");
				for(i = 0; i < 4; i++)
					MotorSetPower(all_motors[i], 1.0f);
			}
			else if(c == 'm')
			{
				unsigned long i;
				UsartWriteLine(&usart0, "Motors at 00%");
				for(i = 0; i < 4; i++)
					MotorSetPower(all_motors[i], 0.0f);
			}
			else if(c == ' ')
			{
				unsigned long i;
				UsartWriteLine(&usart0, "Motors at 50%");
				for(i = 0; i < 4; i++)
					MotorSetPower(all_motors[i], 0.5f);
			}
	#endif
		}
	}
}
