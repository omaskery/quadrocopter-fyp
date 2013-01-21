
#include "includes.h"
#include "usart.h"
#include "motor.h"
#include "globals.h"
#include "init.h"

int main(void)
{
	float percent = 0.0f;
	unsigned long delay = 0;
	
	InitialiseSystem();
	UsartWriteLine(&usart0, "Initialisation Complete");
	
	MotorSetPower(&motorA, 1.00f);
	MotorSetPower(&motorB, 0.75f);
	MotorSetPower(&motorC, 0.25f);
	MotorSetPower(&motorD, 0.00f);
	
	while(1)
	{
		if(UsartIsDataWaiting(&usart0))
		{
			UsartPut(&usart0, UsartGet(&usart0));
		}
		
		if(delay++ >= 2000000)
		{
			delay = 0;
			MotorSetPower(&motorC, percent);
			percent += 0.1f;
			if(percent > 1.0f)
				percent = 0.0f;
		}
	}
}
