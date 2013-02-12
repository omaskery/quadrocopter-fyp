#ifndef GLOBALS_H
#define GLOBALS_H

// compiler switch that builds system for motor configuration
// comment out for normal operation
//#define MOTOR_CONFIG

// the baudrate for the USB usart
#define	USART0_BAUDRATE		256000

// the buffer sizes of the transmit/receive queues
#define	RX_BUFFER_SIZE		256
#define	TX_BUFFER_SIZE		1024

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
