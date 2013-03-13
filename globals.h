#ifndef GLOBALS_H
#define GLOBALS_H

// compiler switch that builds system for motor configuration
// comment out for normal operation
//#define MOTOR_CONFIG
//#define TEST_MODE

// if defined writes out data from motion subsystem, otherwise does not
#define DEBUG_MOTION

// sets wheter acceleration values are passed into the PID rather than absolute rotation
//#define USE_ACCELERATION

// the baudrate for the USB usart
#define	USART0_BAUDRATE		115200

// the buffer sizes of the transmit/receive queues
#define	RX_BUFFER_SIZE		256
#define	TX_BUFFER_SIZE		2048

// the buffer size for the I2C interface
#define I2C_BUFFER_SIZE		32
// the address of the MPU6050
#define MPU6050_ADDR			0x68

// the period of the PWM outputs in milliseconds
#define PWM_PERIOD				20 // ms

// the number of averages to take for the offset subtraction
#define LONG_AVERAGING		5000
#define SHORT_AVERAGING		16

#endif
