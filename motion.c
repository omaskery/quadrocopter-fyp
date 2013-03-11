#include "motion.h"
#include "system.h"

rotation orientation;

rotation_component gyroOffsets;

uint16_t calibrationCount;
int calibrated;

rotation_component shortAverage;
uint16_t readingCount;

void _MotionCallback(sensor*);

void MotionInitialise(rotation *this, sensor *_sensor)
{
	this->angle.x = 0;
	this->angle.y = 0;
	this->angle.z = 0;
	this->rate.x = 0;
	this->rate.y = 0;
	this->rate.z = 0;
	_sensor->callback = _MotionCallback;
	
	gyroOffsets.x = 0;
	gyroOffsets.y = 0;
	gyroOffsets.z = 0;
	
	calibrationCount = 0;
	calibrated = 0;
	
	shortAverage.x = 0;
	shortAverage.y = 0;
	shortAverage.z = 0;
	
	readingCount = 0;
}

int  MotionIsCalibrated(void)
{
	return calibrated;
}

void _MotionCallback(sensor *_sensor)
{
	//const int zeroDriftMax = 10;
	//static int zeroDrift = 0;
	
	if(calibrated)
	{
		const float dtCoefficient = 1.0f / 200.0f;
		//const float driftCoefficient = 0.999f;
		const int32_t lsbDivider = 65;
		rotation_component latest;
		//rotation_component display;
		
		latest.x = _sensor->gyro.x;
		latest.y = _sensor->gyro.y;
		latest.z = _sensor->gyro.z;
		
 		latest.x -= gyroOffsets.x;
 		latest.y -= gyroOffsets.y;
 		latest.z -= gyroOffsets.z;
		
		latest.x /= lsbDivider;
		latest.y /= lsbDivider;
		latest.z /= lsbDivider;
		
		orientation.rate = latest;
		orientation.angle.x += (orientation.rate.x * dtCoefficient);
		orientation.angle.y += (orientation.rate.y * dtCoefficient);
		orientation.angle.z += (orientation.rate.z * dtCoefficient);
		
		/*
		// drift towads zero behaviour, supervisor thinks this is a bad idea!
		if(zeroDrift++ >= zeroDriftMax)
		{
			zeroDrift = 0;
			orientation.angle.x *= driftCoefficient;
			orientation.angle.y *= driftCoefficient;
			orientation.angle.z *= driftCoefficient;
		}
		*/
		
		/*
		display = orientation.rate;
		UsartPutStr(&usart0, "g/s\t");
		UsartWriteInt32(&usart0, display.x);
		UsartPut(&usart0, '\t');
		UsartWriteInt32(&usart0, display.y);
		UsartPut(&usart0, '\t');
		UsartWriteInt32(&usart0, display.z);
		UsartPutNewLine(&usart0);
		*/
		
#define USE_HEX
		
#ifndef USE_HEX
		UsartPutStr(&usart0, "data\t");
		UsartWriteInt32(&usart0, (sys.milliseconds));
		UsartPut(&usart0, '\t');
		UsartWriteInt32(&usart0, (int32_t)(orientation.angle.x * 100.0f));
		UsartPut(&usart0, '\t');
		UsartWriteInt32(&usart0, (int32_t)(orientation.angle.y * 100.0f));
		UsartPut(&usart0, '\t');
		UsartWriteInt32(&usart0, (int32_t)(orientation.angle.z * 100.0f));
		UsartPut(&usart0, '\t');
		UsartWriteInt32(&usart0, (orientation.rate.x));
		UsartPut(&usart0, '\t');
		UsartWriteInt32(&usart0, (orientation.rate.y));
		UsartPut(&usart0, '\t');
		UsartWriteInt32(&usart0, (orientation.rate.z));
		UsartPutNewLine(&usart0);
#else
		UsartPutStr(&usart0, "data\t");
		UsartWriteHex32(&usart0, (sys.milliseconds));
		UsartPut(&usart0, '\t');
		UsartWriteHex16(&usart0, (int32_t)(orientation.angle.x * 100.0f));
		UsartPut(&usart0, '\t');
		UsartWriteHex16(&usart0, (int32_t)(orientation.angle.y * 100.0f));
		UsartPut(&usart0, '\t');
		UsartWriteHex16(&usart0, (int32_t)(orientation.angle.z * 100.0f));
		UsartPut(&usart0, '\t');
		UsartWriteHex16(&usart0, (orientation.rate.x));
		UsartPut(&usart0, '\t');
		UsartWriteHex16(&usart0, (orientation.rate.y));
		UsartPut(&usart0, '\t');
		UsartWriteHex16(&usart0, (orientation.rate.z));
		UsartPutNewLine(&usart0);
#endif
	}
	else
	{
		gyroOffsets.x += _sensor->gyro.x;
		gyroOffsets.y += _sensor->gyro.y;
		gyroOffsets.z += _sensor->gyro.z;
		
		if(++calibrationCount >= LONG_AVERAGING)
		{
			calibrated = 1;
			
			gyroOffsets.x = gyroOffsets.x / LONG_AVERAGING;
			gyroOffsets.y = gyroOffsets.y / LONG_AVERAGING;
			gyroOffsets.z = gyroOffsets.z / LONG_AVERAGING;
		}
	}
}
