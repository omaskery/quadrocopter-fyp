#ifndef USART_H
#define USART_H

#include "includes.h"
#include "queue.h"

// structure to represent a USART peripheral
typedef struct usart_t
{
	unsigned long usartBase;		// the base address for the peripheral
	queue rxQueue, txQueue;			// the transmit and receive cyclic queues for this usart
	int transmitting:1;					// flag to indicate whether the transmit interrupt is enabled
	int rxFlag:1;								// flag set when new data has arrived
} usart;

// usart0 object
extern volatile usart usart0;

// interrupt for usart0
void UsartInterrupt0(void);
// initialise a usart0, takes a usart* (this), the base address of the usart, the baudratte and a pointer to two buffer objects for rx and tx
void UsartInitialise(volatile usart*, unsigned long, unsigned long, buffer*, buffer*);

// getter for querying whether data is buffered in the RX queue
int  UsartIsDataWaiting(const volatile usart*);
// get a character from the RX buffer
char UsartGet(volatile usart*);
// transmit (or queue) a single character
int  UsartPut(volatile usart*, char);
// transmit a string delimited by '\0'
void UsartPutStr(volatile usart*, const char*);
// transmit an array of bytes, determined by the length
void UsartPutData(volatile usart*, const char*, unsigned long);
// transmit an array of bytes, described by the buffer object
void UsartPutBuffer(volatile usart*, buffer*);
// transmit a "\r\n"
void UsartPutNewLine(volatile usart*);
// transmit a string delimited by '\0' followed by "\r\n"
void UsartWriteLine(volatile usart*, const char*);

// transmit values as strings
void UsartWriteNumber(volatile usart *this, int _number, int _bits);
// abstractions for writing number:
void UsartWriteInt8(volatile usart *this, int8_t _number);
void UsartWriteInt16(volatile usart *this, int16_t _number);
void UsartWriteInt32(volatile usart *this, int32_t _number);
void UsartWriteHex4(volatile usart *this, uint8_t _number);
void UsartWriteHex8(volatile usart *this, uint8_t _number);
void UsartWriteHex16(volatile usart *this, uint16_t _number);
void UsartWriteHex32(volatile usart *this, uint32_t _number);

// transmit a formatted string in the same way as printf() would work
// NOTE: there appears to be some bug with vsnprintf() which corrupts global memory!? disabling for now
//void UsartFormat(usart*, const char*, ...);

#endif
