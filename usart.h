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
extern usart usart0;

// interrupt for usart0
void UsartInterrupt0(void);
// initialise a usart0, takes a usart* (this), the base address of the usart, the baudratte and a pointer to two buffer objects for rx and tx
void UsartInitialise(usart*, unsigned long, unsigned long, buffer*, buffer*);

// getter for querying whether data is buffered in the RX queue
int  UsartIsDataWaiting(const usart*);
// get a character from the RX buffer
char UsartGet(usart*);
// transmit (or queue) a single character
int  UsartPut(usart*, char);
// transmit a string delimited by '\0'
void UsartPutStr(usart*, char*);
// transmit an array of bytes, determined by the length
void UsartPutData(usart*, char*, unsigned long);
// transmit an array of bytes, described by the buffer object
void UsartPutBuffer(usart*, buffer*);
// transmit a "\r\n"
void UsartPutNewLine(usart*);
// transmit a string delimited by '\0' followed by "\r\n"
void UsartWriteLine(usart*, char*);
// transmit a formatted string in the same way as printf() would work
void UsartFormat(usart*, char*, ...);

#endif
