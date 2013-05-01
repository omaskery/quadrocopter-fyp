#include "motion.h"
#include "system.h"

rotation orientation;

rotation_component gyroOffsets;
rotation_component accelOffsets;

uint16_t calibrationCount;
int calibrated;

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
	accelOffsets.x = 0;
	accelOffsets.y = 0;
	accelOffsets.z = 0;
	
	calibrationCount = 0;
	calibrated = 0;
	
	readingCount = 0;
}

int  MotionIsCalibrated(void)
{
	return calibrated;
}

void _MotionCallback(sensor *_sensor)
{
	if(calibrated)
	{
		const float dtCoefficient = 1.0f / 200.0f;
		const float lsbGyroDivider = 65.0f;
		const float lsbAccelDivider = 163.840f;
		const float accelGravity = 100.0f;
		const float complimentaryRatio = 0.999f;
		
		float xAccelAngle, yAccelAngle;
		
		rotation_component latestGyro;
		rotation_component latestAccel;
		rotation_component latestAccelSq;
		
		latestGyro.x = _sensor->gyro.x;
		latestGyro.y = _sensor->gyro.y;
		latestGyro.z = _sensor->gyro.z;
		
 		latestGyro.x -= gyroOffsets.x;
 		latestGyro.y -= gyroOffsets.y;
 		latestGyro.z -= gyroOffsets.z;
		
		latestGyro.x /= lsbGyroDivider;
		latestGyro.y /= lsbGyroDivider;
		latestGyro.z /= lsbGyroDivider;
		
		latestAccel.x = _sensor->accel.x;
		latestAccel.y = _sensor->accel.y;
		latestAccel.z = _sensor->accel.z;
		
		latestAccel.x -= accelOffsets.x;
		latestAccel.y -= accelOffsets.y;
		latestAccel.z -= accelOffsets.z;
		
		latestAccel.x /= lsbAccelDivider;
		latestAccel.y /= lsbAccelDivider;
		latestAccel.z /= lsbAccelDivider;
		
		latestAccel.z += accelGravity;
		
		latestAccelSq.x = latestAccel.x * latestAccel.x;
		latestAccelSq.y = latestAccel.y * latestAccel.y;
		latestAccelSq.z = latestAccel.z * latestAccel.z;
		
		yAccelAngle = -atan(latestAccel.x / sqrt(latestAccelSq.y + latestAccelSq.z)) * 180.0 / 3.14159;
		xAccelAngle = atan(latestAccel.y / sqrt(latestAccelSq.x + latestAccelSq.z))	* 180.0 / 3.14159;
		
		orientation.rate = latestGyro;
		
		orientation.angle.x = (complimentaryRatio*(orientation.angle.x + orientation.rate.x * dtCoefficient)) + ((1.0f - complimentaryRatio) * xAccelAngle);
		orientation.angle.y = (complimentaryRatio*(orientation.angle.y + orientation.rate.y * dtCoefficient)) + ((1.0f - complimentaryRatio) * yAccelAngle);
		orientation.angle.z += (orientation.rate.z * dtCoefficient);

#ifdef DEBUG_MOTION

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
#else // USE_HEX
		UsartPutStr(&usart0, "data\t");
		UsartWriteHex32(&usart0, (sys.milliseconds));
		UsartPut(&usart0, '\t');
		UsartWriteHex16(&usart0, (int32_t)(orientation.angle.x * 100.0f));
		UsartPut(&usart0, '\t');
		UsartWriteHex16(&usart0, (int32_t)(orientation.angle.y * 100.0f));
		UsartPut(&usart0, '\t');
		UsartWriteHex16(&usart0, (int32_t)(orientation.angle.z * 100.0f));
		UsartPut(&usart0, '\t');
		UsartWriteHex16(&usart0, (int32_t)(xAccelAngle * 100.0f));
		UsartPut(&usart0, '\t');
		UsartWriteHex16(&usart0, (int32_t)(yAccelAngle * 100.0f));
		UsartPut(&usart0, '\t');
		UsartWriteHex16(&usart0, (int32_t)(latestAccel.z * 100.0f));
		UsartPutNewLine(&usart0);
#endif // USE_HEX

#endif // DEBUG_MOTION
	}
	else
	{
		gyroOffsets.x += _sensor->gyro.x;
		gyroOffsets.y += _sensor->gyro.y;
		gyroOffsets.z += _sensor->gyro.z;
		accelOffsets.x += _sensor->accel.x;
		accelOffsets.y += _sensor->accel.y;
		accelOffsets.z += _sensor->accel.z;
		
		if(++calibrationCount >= LONG_AVERAGING)
		{
			calibrated = 1;
			
			gyroOffsets.x = gyroOffsets.x / LONG_AVERAGING;
			gyroOffsets.y = gyroOffsets.y / LONG_AVERAGING;
			gyroOffsets.z = gyroOffsets.z / LONG_AVERAGING;
			
			accelOffsets.x /= LONG_AVERAGING;
			accelOffsets.y /= LONG_AVERAGING;
			accelOffsets.z /= LONG_AVERAGING;
		}
	}
}
