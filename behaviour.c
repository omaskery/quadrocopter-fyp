#include "behaviour.h"
#include "motor.h"
#include "usart.h"
#include "iic.h"
#include "sensor.h"
#include "motion.h"
#include "flight.h"

// cough, need to write an abstraction for this...
#include "drivers/rit128x96x4.h"

// private modes:
void _Mode_MotorStart(system_status*);

const unsigned long one_second = 400; // 'thinking' is 400Hz
const unsigned long five_msecond = 2;

void _Mode_Warmup(system_status *this)
{
	static unsigned long delay = 0;
	static int testPassed = 0;
	
	if(this->modeChanged)
	{
		RIT128x96x4Clear();
		RIT128x96x4StringDraw("Warmup period.", 5, 5, 15);
		UsartPutStr(&usart0, "Warmup delay... ");
		delay = 0;
		testPassed = 0;
		SensorOpTestID(&mpu6050);
		MotorSetPower(&motorA, 0.0f);
		MotorSetPower(&motorB, 0.0f);
		MotorSetPower(&motorC, 0.0f);
		MotorSetPower(&motorD, 0.0f);
	}
	
	if(mpu6050.validity == SENSOR_INVALID)
	{
		UsartWriteLine(&usart0, "sensor test failed! Aborting.");
		SystemKillEnable(this);
	}
	else if(!testPassed && mpu6050.validity == SENSOR_VALID)
	{
		testPassed = 1;
		UsartPutStr(&usart0, "(sensor test passed)... ");
	}
	
	if(delay > (10 * one_second))
	{
		if(testPassed)
		{
			SensorOpConfigure(&mpu6050);
			SystemSetMode(this, _Mode_MotorStart);
			RIT128x96x4StringDraw("Warmup complete.", 5, 15, 15);
			UsartWriteLine(&usart0, "done.");
			MotorsEnableAll(all_motors, 4);
		}
	}
	else
	{
		delay++;
	}
}

// starts motors after warmup phase
void _Mode_MotorStart(system_status *this)
{
	const unsigned long startDelay = five_msecond * 50;
	static unsigned long delay = startDelay;
	static int motorIndex = 0;
	static int calibrated = 0;
	static int started = 0;
	
	if(!calibrated)
	{
		if(MotionIsCalibrated())
		{
			calibrated = 1;
			UsartWriteLine(&usart0, "Motion calibrated!");
			RIT128x96x4StringDraw("Sensor calibrated! ", 5, 35, 15);
		}
		else
		{
			SensorOpGetMotion6(&mpu6050);
		}
	}
	
	if(this->modeChanged)
	{
		UsartWriteLine(&usart0, "Motor start & motion calibration!");
		delay = startDelay;
		motorIndex = 0;
		calibrated = 0;
		started = 0;
		RIT128x96x4StringDraw("Sensor uncalibrated", 5, 35, 15);
	}
	
	if(!started && (motorIndex < 4 && delay++ >= startDelay))
	{
		delay = 0;
		
		UsartPutStr(&usart0, "Starting motor: ");
		UsartWriteInt32(&usart0, motorIndex);
		UsartPutNewLine(&usart0);
		
		MotorSetPower(all_motors[motorIndex++], 0.05f);
		switch(motorIndex)
		{
		case 0: RIT128x96x4StringDraw("Starting motor: front", 5, 25, 15); break;
		case 1: RIT128x96x4StringDraw("Starting motor: right", 5, 25, 15); break;
		case 2: RIT128x96x4StringDraw("Starting motor: back ",  5, 25, 15); break;
		case 3: RIT128x96x4StringDraw("Starting motor: left ",  5, 25, 15); break;
		}
		
		if(motorIndex >= 4)
		{
			if(MotionIsCalibrated())
				UsartWriteLine(&usart0, "Motors running!");
			else
				UsartWriteLine(&usart0, "Motors running, waiting for motion calibration!");
			started = 1;
		}
	}
	
	if(started && calibrated)
	{
		RIT128x96x4Clear();
		UsartWriteLine(&usart0, "System ready!");
		SystemSetMode(this, _Mode_Running);
	}
}

void _Mode_Running(system_status *this)
{
	const int realRate = 1;
	const int everyOther = realRate;
	static int toggle = 0;
	
	if(this->modeChanged)
	{
		UsartWriteLine(&usart0, "Running!");
		FlightSetThrust(&flight_module, 0.10);
	}
	
	if(toggle ++ >= everyOther)
	{
		toggle = 0;
		SensorOpGetMotion6(&mpu6050);
	}
	
	FlightUpdate(&flight_module, &orientation);
}

void _Mode_Dead(system_status *this)
{
	if(this->modeChanged)
	{
		RIT128x96x4Clear();
		RIT128x96x4StringDraw("System killed", 5, 5, 15);
		RIT128x96x4StringDraw("Waiting for 'OK'", 5, 15, 15);
	}
	
	if(!this->killflag)
	{
		SystemSetMode(this, _Mode_Warmup);
	}
}

void _Mode_MotorConfigure(system_status *this)
{
	if(this->modeChanged)
	{
		int i;
		MotorsEnableAll(all_motors, 4);
		for(i = 0; i < 4; i++)
			MotorSetPower(all_motors[i], 1.0f);
	}
}

void _Mode_TestMode(system_status *this)
{
	const int realRate = 1;
	const int everyOther = realRate;
	static int toggle = 0;
	static int untilStart = 0;
	static int started = 0;
	
	if(this->modeChanged)
	{
		int i;
		MotorsEnableAll(all_motors, 4);
		for(i = 0; i < 4; i++)
			MotorSetPower(all_motors[i], 0.0f);
		
		untilStart = 2 * one_second;
		started = 0;
		UsartWriteLine(&usart0, "entered Test Mode, motors enabled");
	}
	
	if(toggle ++ >= everyOther)
	{
		toggle = 0;
		SensorOpGetMotion6(&mpu6050);
		// emulate sensor output for usart debugging:
		//UsartWriteLine(&usart0, "data\tffff\tffff\tff\tf");
	}
	
	if(untilStart > 0)
	{
		untilStart --;
		if(untilStart == 0)
		{
			UsartWriteLine(&usart0, "start delay complete, waiting for calibration!");
		}
	}
	else
	{
		if(!started && MotionIsCalibrated())
		{
			started = 1;
			MotorSetPower(&motorD, 1.0f);
			MotorSetPower(&motorB, 0.0f);
			UsartWriteLine(&usart0, "motors away!");
		}
	}
}
