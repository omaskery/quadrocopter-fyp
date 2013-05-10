#ifndef SENSOR_H
#define SENSOR_H

#include "../peripheral_drivers/iic.h"
#include "../peripheral_drivers/usart.h"

// enumeration describing the validity of the sensor (whether it has been tested)
enum SENSOR_VALIDITY
{
	SENSOR_UNTESTED,		// the sensor has yet to respond to the sensor test (or be queried)
	SENSOR_VALID,				// the sensor has responded correctly to the test query
	SENSOR_INVALID			// the sensor has responded incorrectly to the test query
};

// structure representing a set of sensor readings for one device
typedef struct sensor_reading_t
{
	int16_t x,y,z;			// xyz components of the sensor reading
} sensor_reading;

struct sensor_t;

// function type definition for callback functions waiting for sensor operations to complete
typedef void (*sensor_callback)(struct sensor_t*);

// struct for representing a sensor device
typedef struct sensor_t
{
	i2c *interface;									// the I2C interface on which the sensor can be reached
	int operation;									// the current operation ID that the sensor is performing (internal detail)
	
	int validity;										// stores SENSOR_VALIDITY enum value
	
	sensor_reading accel;						// stores latest readings from accelerometer
	sensor_reading gyro;						// stores latest readings from gyroscope
	
	sensor_callback callback;				// callback method for this sensor when an operation completes
} sensor;

// default sensor, the MPU6050
extern sensor mpu6050;

// initialise a sensor, taking a 'this', an i2c interface and an address for the device
void SensorInitialise(sensor*, i2c*, unsigned char);

// initiate the "Test ID" operation on the sensor (sets the validity value)
int  SensorOpTestID(sensor*);
// initiate the "Configure" operation on the sensor, initialises values over I2C
int  SensorOpConfigure(sensor*);
// initiate the "GetMotion6" operation on the sensor, causing it to read accel/gyro values
int  SensorOpGetMotion6(sensor*);
// convenience method for dumping the latest accel/gyro values from a sensor to a USART link
void SensorDumpReading(sensor*, usart*);

#endif
