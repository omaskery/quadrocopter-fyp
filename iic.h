#ifndef IIC_H
#define IIC_H

#include "includes.h"
#include "globals.h"
#include "queue.h"

struct i2c_t;

// callback for people to listen for responses, takes 'this' and number of expected characters
typedef void (*i2c_callback)(struct i2c_t*,char);

// represents an I2C peripheral
typedef struct i2c_t
{
	unsigned char slave;			// the address of the slave device
	unsigned long base;				// the base address of the I2C peripheral
	queue txQueue;						// the transmission queue
	queue rxQueue;						// the receive queue
	unsigned char count;			// number of characters the current operation involves
	
	int busy:1;								// busy flag for preventing overlapping operations
	int receiving:1;					// flag indicating wheter the I2C is receiving or not
	int rxFlag:1;							// flag set to 1 when a I2C receive is completed, cleared on init of RX
	
	char expectedResponse;		// number of characters expected in response to a send, if non zero results in receive
	
	i2c_callback callback;		// callback for when a request is complete
} i2c;

extern i2c i2c0;

// interrupt handler for I2C 0
void IicTransactionComplete0(void);
// initialise an i2c device with it's base address and a pointer to an RX and TX buffer
void IicInitialise(i2c*, unsigned long, buffer*, buffer*);
// set the slave address of the I2C master
void IicSetSlave(i2c*, unsigned char);
// puts data into the I2C TX buffer ready for transmission
void IicPutData(i2c*, unsigned char);
// initiates a transmission on I2C
void IicStartSend(i2c*);
// initiates a transmission with a callback
void IicStartSendWithHook(i2c*, i2c_callback);
// initiates a request, a transmission is performed, upon completion a receive is initiated immediately
void IicStartRequest(i2c*, unsigned char);
// initiates request, transmit query, receives response, calls callback to handle results
void IicStartRequestWithHook(i2c*, unsigned char, i2c_callback);
// initiates a receive on the I2C
void IicStartReceive(i2c*, unsigned char);
// initiates a receive with a callback
void IicStartReceiveWithHook(i2c*, unsigned char, i2c_callback);

#endif
