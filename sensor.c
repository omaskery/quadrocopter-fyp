#include "sensor.h"

sensor mpu6050;

#define MPU6050_WHO_AM_I									0x75
#define MPU6050_DEVICE_ID									0x34
#define MPU6050_ACCEL_XOUT								0x3B

#define MPU6050_RA_PWR_MGMT_1							0x6B
#define MPU6050_CLOCK_PLL_XGYRO						0x01
#define MPU6050_PWR1_SLEEP_BIT						6
#define MPU6050_PWR1_CLKSEL_BIT						2
#define MPU6050_PWR1_CLKSEL_LENGTH				3

#define MPU6050_RA_GYRO_CONFIG						0x1B
#define MPU6050_GYRO_FS_250								0x00
#define MPU6050_GCONFIG_FS_SEL_BIT				4
#define MPU6050_FS_SEL_LENGT							2

#define MPU6050_RA_ACCEL_CONFIG						0x1C
#define MPU6050_ACCEL_FS_2								0x00
#define MPU6050_ACONFIG_AFS_SEL_BIT				4
#define MPU6050_ACONFIG_AFS_SEL_LENGTH		2

enum SENSOR_OPERATION
{
	SENSE_OP_NONE = 0,
	SENSE_OP_INIT_TEST,
	SENSE_OP_CONFIG_POWER,
	SENSE_OP_CONFIG_GYRO,
	SENSE_OP_CONFIG_ACCEL,
	SENSE_OP_GET_MOTION6,
	SENSE_OP_MAX
};

void _SensorCallback(sensor*, i2c*, char);
void _SensorQueryRegister(sensor*, unsigned char, char);
void _SensorWriteRegister(sensor*, unsigned char, unsigned char);
int  _SensorOperation(sensor*, int);
int  _SensorOpConfigure_Power(sensor*);
int  _SensorOpConfigure_Gyro(sensor*);
int  _SensorOpConfigure_Accel(sensor*);

void _SensorCallback(sensor *this, i2c *_interface, char _amount)
{
	char buffer[16];
	int i;
	
	char op = this->operation;
	this->operation = SENSE_OP_NONE;
	
	switch(op)
	{
	case SENSE_OP_NONE:
		{
			UsartWriteLine(&usart0, "sensor callback with _NO_ operation");
		} break;
	case SENSE_OP_INIT_TEST:
		{
			buffer[0] = QueuePop(&_interface->rxQueue);
			buffer[0] = (buffer[0] >> 1) & 0x3F;
			if(buffer[0] == MPU6050_DEVICE_ID)
			{
				this->validity = SENSOR_VALID;
			}
			else
			{
				this->validity = SENSOR_INVALID;
			}
		} break;
	case SENSE_OP_CONFIG_POWER:
		{
			UsartWriteLine(&usart0, "sensor:power configured");
			_SensorOpConfigure_Gyro(this);
		} break;
	case SENSE_OP_CONFIG_GYRO:
		{
			UsartWriteLine(&usart0, "sensor:gyro configured");
			_SensorOpConfigure_Accel(this);
		} break;
	case SENSE_OP_CONFIG_ACCEL:
		{
			UsartWriteLine(&usart0, "sensor:accel configured");
		} break;
	case SENSE_OP_GET_MOTION6:
		{
			for(i = 0; i < 14; i++)
				buffer[i] = QueuePop(&_interface->rxQueue);
			
			this->accel.x = ((int16_t)((buffer[ 0] << 8) | buffer[ 1]));
			this->accel.y = ((int16_t)((buffer[ 2] << 8) | buffer[ 3]));
			this->accel.z = ((int16_t)((buffer[ 4] << 8) | buffer[ 5]));
			this->gyro.x  = ((int16_t)((buffer[ 8] << 8) | buffer[ 9]));
			this->gyro.y  = ((int16_t)((buffer[10] << 8) | buffer[11]));
			this->gyro.z  = ((int16_t)((buffer[12] << 8) | buffer[13]));
			
			if(this->callback != NULL)
			{
				this->callback(this);
			}
			
			//SensorDumpReading(this, &usart0);
		} break;
	default:
		{
			UsartWriteLine(&usart0, "sensor callback with unknown operation");
		}
	}
}

void SensorCallback(i2c *_interface, char _amount)
{
	_SensorCallback(&mpu6050, _interface, _amount);
}

void _SensorQueryRegister(sensor *this, unsigned char _register, char _count)
{
	IicPutData(this->interface, _register);
	IicStartRequestWithHook(this->interface, _count, SensorCallback);
}

void _SensorWriteRegister(sensor *this, unsigned char _register, unsigned char _value)
{
	IicPutData(this->interface, _register);
	IicPutData(this->interface, _value);
	IicStartSendWithHook(this->interface, SensorCallback);
}

int  _SensorOperation(sensor *this, int _operation)
{
	if(this->operation != SENSE_OP_NONE)
		return 0;
	this->operation = _operation;
	return 1;
}

void SensorInitialise(sensor *this, i2c *_interface, unsigned char _address)
{
	this->interface = _interface;
	this->operation = SENSE_OP_NONE;
	this->validity = SENSOR_UNTESTED;
	this->callback = NULL;
	
	IicSetSlave(this->interface, _address);
}

int  SensorOpTestID(sensor *this)
{
	if(_SensorOperation(this, SENSE_OP_INIT_TEST))
	{
		_SensorQueryRegister(this, MPU6050_WHO_AM_I, 1);
		return 1;
	}
	return 0;
}

int  _SensorOpConfigure_Power(sensor *this)
{
	if(_SensorOperation(this, SENSE_OP_CONFIG_POWER))
	{
		_SensorWriteRegister(this, MPU6050_RA_PWR_MGMT_1, 0x01);
		return 1;
	}
	return 0;
}

int  _SensorOpConfigure_Gyro(sensor *this)
{
	if(_SensorOperation(this, SENSE_OP_CONFIG_GYRO))
	{
		_SensorWriteRegister(this, MPU6050_RA_GYRO_CONFIG, 0x08);
		return 1;
	}
	return 0;
}

int  _SensorOpConfigure_Accel(sensor *this)
{
	if(_SensorOperation(this, SENSE_OP_CONFIG_ACCEL))
	{
		_SensorWriteRegister(this, MPU6050_RA_ACCEL_CONFIG, 0x00);
		return 1;
	}
	return 0;
}

int  SensorOpConfigure(sensor *this)
{
	return _SensorOpConfigure_Power(this);
}

int  SensorOpGetMotion6(sensor *this)
{
	if(_SensorOperation(this, SENSE_OP_GET_MOTION6))
	{
		_SensorQueryRegister(this, MPU6050_ACCEL_XOUT, 14);
		return 1;
	}
	return 0;
}

void SensorDumpReading(sensor *this, usart *_usart)
{
	long checksum = 0;
	
	checksum += this->accel.x;
	checksum += this->accel.y;
	checksum += this->accel.z;
	checksum += this->gyro.x;
	checksum += this->gyro.y;
	checksum += this->gyro.z;
	
	UsartPut(_usart, '\a');
	UsartPut(_usart, (char)(sizeof(sensor_reading)*2) + sizeof(checksum));
	UsartPutData(_usart, (char*)&this->accel, sizeof(this->accel));
	UsartPutData(_usart, (char*)&this->gyro, sizeof(this->gyro));
	UsartPutData(_usart, (char*)&checksum, sizeof(checksum));
}
