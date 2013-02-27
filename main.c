
#include "includes.h"
#include "usart.h"
#include "motor.h"
#include "globals.h"
#include "system.h"
#include "init.h"
#include "flight.h"

void system_think(void)
{
	IntMasterDisable();
	
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	
	SystemThink(&sys);
	
	IntMasterEnable();
}

void timer_think(void)
{
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	
	sys.milliseconds ++;
}

int main(void)
{
	InitialiseSystem();
	
	while(1)
	{
		if(UsartIsDataWaiting(&usart0))
		{
			char c = UsartGet(&usart0);

			if(sys.killflag == 0)
			{
				if(c == 'k')
				{
					SystemKillEnable(&sys);
					UsartWriteLine(&usart0, "Killflag engaged");
				}
				else if(c == 'w')
				{
					FlightSetThrust(&flight_module, flight_module.thrust + 0.05f);
					UsartWriteLine(&usart0, "Increasing thrust");
				}
				else if(c == 's')
				{
					FlightSetThrust(&flight_module, flight_module.thrust - 0.05f);
					UsartWriteLine(&usart0, "Decreasing thrust");
				}
				else if(c == 'z')
				{
					orientation.angle.x = 0.0f;
					orientation.angle.y = 0.0f;
					orientation.angle.z = 0.0f;
					UsartWriteLine(&usart0, "Zeroing orientation");
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
			else
			{
				if(c == 'K' && sys.killflag)
				{
					SystemKillDisable(&sys);
					UsartWriteLine(&usart0, "Killflag disengaged");
				}
			}
		}
	}
}
