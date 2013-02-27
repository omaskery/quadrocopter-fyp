#include "usart.h"
#include "globals.h"

volatile usart usart0;

void UsartEnableTxInterrupt(volatile usart *this)
{
	UARTIntEnable(this->usartBase, UART_INT_TX);
	this->transmitting = 1;
}

void UsartDisableTxInterrupt(volatile usart *this)
{
	UARTIntDisable(this->usartBase, UART_INT_TX);
	this->transmitting = 0;
}

void UsartPumpTx(volatile usart *this)
{
	char buffered;
	
	if(!UARTSpaceAvail(this->usartBase))
		return;
	
	buffered = QueuePop(&this->txQueue);
	UARTCharPutNonBlocking(this->usartBase, buffered);
	UsartEnableTxInterrupt(this);
}

void UsartInterrupt(volatile usart *this)
{
	unsigned long status;
	char buffered;
	
	status = UARTIntStatus(this->usartBase, true);
	
	UARTIntClear(this->usartBase, status);
	
	while(UARTCharsAvail(this->usartBase))
	{
		buffered = UARTCharGetNonBlocking(this->usartBase);
		QueuePush(&this->rxQueue, buffered);
		this->rxFlag = 1;
	}
	
	while(!QueueIsEmpty(&this->txQueue) && UARTSpaceAvail(this->usartBase))
	{
		UsartPumpTx(this);
	}
	
	if(QueueIsEmpty(&this->txQueue) && !UARTBusy(this->usartBase) && this->transmitting)
	{
		UsartDisableTxInterrupt(this);
	}
}

void UsartInterrupt0(void)
{
	UsartInterrupt(&usart0);
}

// blatant hack, could just publically expose method from uart.c in driverlib, but don't really want to mess with it
unsigned long UsartInterruptNumberFromBase(unsigned long _usartBase)
{
	switch(_usartBase)
	{
		case UART0_BASE: return INT_UART0;
		case UART1_BASE: return INT_UART1;
		case UART2_BASE: return INT_UART2;
		case UART3_BASE: return INT_UART3;
		case UART4_BASE: return INT_UART4;
		case UART5_BASE: return INT_UART5;
		case UART6_BASE: return INT_UART6;
		case UART7_BASE: return INT_UART7;
	}
	return 0;
}

void UsartInitialise(volatile usart *this, unsigned long _usartBase, unsigned long _baudrate, buffer *_receive, buffer *_transmit)
{
	unsigned long interruptBase;
	
	this->rxFlag = 0;
	this->transmitting = 0;
	this->usartBase = _usartBase;
	interruptBase = UsartInterruptNumberFromBase(this->usartBase);
	
	UARTConfigSetExpClk(this->usartBase, SysCtlClockGet(), _baudrate,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
	IntEnable(interruptBase);
	UARTFIFODisable(this->usartBase);
  UARTIntEnable(this->usartBase, UART_INT_RX);
	
	QueueInitialise(&this->rxQueue, _receive);
	QueueInitialise(&this->txQueue, _transmit);
}

int  UsartIsDataWaiting(const volatile usart* this)
{
	return !QueueIsEmpty(&this->rxQueue);
}

char UsartGet(volatile usart *this)
{
	return QueuePop(&this->rxQueue);
}

int  UsartPut(volatile usart *this, char _data)
{
	int result;
	
	// force code to wait, waiting for space, should highlight any timing issues pretty sharpish!
	while(QueueIsFull(&this->txQueue));
	
	result = QueuePush(&this->txQueue, _data);
	
	UsartPumpTx(this);
	
	return result;
}

void UsartPutStr(volatile usart *this, const char *_str)
{
	while(*_str)
		UsartPut(this, *(_str++));
}

void UsartPutData(volatile usart *this, const char *_data, unsigned long _length)
{
	unsigned int i;
	for(i = 0; i < _length; i++)
		UsartPut(this, _data[i]);
}

void UsartPutBuffer(volatile usart *this, buffer *_buffer)
{
	UsartPutData(this, _buffer->data, _buffer->length);
}

void UsartPutNewLine(volatile usart *this)
{
	UsartPutStr(this, "\r\n");
}

void UsartWriteLine(volatile usart *this, const char *_line)
{
	UsartPutStr(this, _line);
	UsartPutNewLine(this);
}

void UsartWriteNumber(volatile usart *this, int _number, int _bits)
{
	int mask = (1 << _bits) - 1;
	int complement = (1 << (_bits-1));
	
	int started = 0;
	int index = 0;
	
	char buffer[10];
	int divider;
	char digit;
	int unit;
	
	for(unit = 0; unit < sizeof(buffer); unit++)
		buffer[unit] = '\0';
	
	_number = _number & mask;
	if(_number & complement)
	{
		_number = (~_number) + 1;
		buffer[index++] = '-';
	}
	
	divider = 1000000000; // 10 ^ 9, the math 'pow' function causes processor to fault, no idea why
	for(unit = sizeof(buffer) - 1; unit >= 0; unit--)
	{
		digit = '0' + ((_number / divider) % 10);
		
		if(!started && (digit != '0' || unit == 0))
			started = 1;
		
		if(started)
			buffer[index++] = digit;
		
		divider /= 10;
	}
	
	UsartPutStr(this, buffer);
}

void UsartWriteInt8(volatile usart *this, int8_t _number)
{
	UsartWriteNumber(this, _number, 8);
}

void UsartWriteInt16(volatile usart *this, int16_t _number)
{
	UsartWriteNumber(this, _number, 16);
}

void UsartWriteInt32(volatile usart *this, int32_t _number)
{
	UsartWriteNumber(this, _number, 32);
}

char NibbleToHex(uint8_t _nibble)
{
	_nibble &= 0x0F;
	if(_nibble <= 9)
		return ('0' + _nibble);
	else if(_nibble >= 10 && _nibble <= 15)
		return ('A' + (_nibble - 10));
	return 'x';
}

void UsartWriteHexX(volatile usart *this, uint32_t _number, int _nibbles)
{
	int nibble;
	int seenNonZero = 0;
	char digit;
	
	char output[9];
	int  outputLength = 0;
	
	for(nibble = _nibbles-1; nibble >= 0; nibble--)
	{
		digit = ((_number >> (nibble * 4)) & 0xF);
		seenNonZero = seenNonZero || (digit != 0);
		if(seenNonZero || (nibble == 0)) {
			output[outputLength++] = NibbleToHex(digit);
		}
	}
	
	output[outputLength] = '\0';
	UsartPutStr(this, output);
}

void UsartWriteHex4(volatile usart *this, uint8_t _number)
{
	UsartWriteHexX(this, _number, 1);
}

void UsartWriteHex8(volatile usart *this, uint8_t _number)
{
	/*
	int seenNonZero = 0;
	UsartWriteHex4(this, _number >> 4, seenNonZero);
	if(((_number >> 4) & 0xF) != 0) seenNonZer = 1;
	UsartWriteHex4(this, _number, seenNonZero);
	*/
	UsartWriteHexX(this, _number, 2);
}

void UsartWriteHex16(volatile usart *this, uint16_t _number)
{
	/*
	UsartWriteHex8(this, _number >> 8, 0);
	UsartWriteHex8(this, _number & 0xFF, _allowZero);
	*/
	UsartWriteHexX(this, _number, 4);
}

void UsartWriteHex32(volatile usart *this, uint32_t _number)
{
	/*
	UsartWriteHex16(this, _number >> 16, 0);
	UsartWriteHex16(this, _number & 0xFFFF, _allowZero);
	*/
	UsartWriteHexX(this, _number, 8);
}

/*
void UsartFormat(usart *this, const char *_format, ...)
{
	const int bufferSize = 128;
	char buffer[bufferSize];
	va_list argptr;
	
	va_start(argptr, _format);
	vsnprintf(buffer, bufferSize - 1, _format, argptr);
	va_end(argptr);
	
	UsartPutStr(this, buffer);
}
*/
