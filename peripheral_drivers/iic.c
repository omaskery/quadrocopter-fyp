#include "iic.h"

#include "usart.h"

i2c i2c0;

void _IicCmd(i2c *this, unsigned long _cmd)
{
	I2CMasterControl(this->base, _cmd);
}

void IicTransactionComplete(i2c *this)
{
	unsigned long status;
	int continuing = 0;
	int remaining = 0;
	int done = 0;
	char buffer;
	
	status = I2CMasterIntStatusEx(this->base, true);
	I2CMasterIntClearEx(this->base, status);
	
	// if we were transmitting
	if(!this->receiving)
	{
		// if we've sent everything, we're done
		if(this->txQueue.count == 0)
		{
			if(this->expectedResponse <= 0)
			{
				done = 1;
			}
			else
			{
				this->busy = 0;
				IicStartReceiveWithHook(this, this->expectedResponse, this->callback);
			}
		}
		else if(this->count > 1)
		{
			// more remains, is this the last byte?
			continuing = (this->txQueue.count > 1);
			buffer = QueuePop(&this->txQueue);
			
			I2CMasterDataPut(this->base, buffer);
			
			if(continuing || this->expectedResponse > 0)
			{
				_IicCmd(this, I2C_MASTER_CMD_BURST_SEND_CONT);
			}
			else
			{
				_IicCmd(this, I2C_MASTER_CMD_BURST_SEND_FINISH);
			}
		}
	}
	// if we're receiving
	else
	{
		buffer = I2CMasterDataGet(this->base);
		QueuePush(&this->rxQueue, buffer);
		
		remaining = (this->count - this->rxQueue.count);
		
		// if more remains, request it
		if(remaining > 0)
		{
			continuing = (remaining > 1);
			
			if(continuing)
			{
				_IicCmd(this, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
			}
			else
			{
				_IicCmd(this, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
			}
		}
		// if no more remains, we're done!
		else
		{
			done = 1;
			this->rxFlag = 1;
		}
	}
	
	// if done, clear the busy flag
	if(done)
	{
		this->busy = 0;
		
		// if a callback is set for this message, call it
		if(this->callback != NULL)
		{
			this->callback(this, this->expectedResponse);
		}
	}
}

void IicTransactionComplete0(void)
{
	IicTransactionComplete(&i2c0);
}

void IicInitialise(i2c *this, unsigned long _base, buffer *_receive, buffer *_transmit)
{
	this->base = _base;
	this->slave = 0;
	this->receiving = 0;
	this->rxFlag = 0;
	this->count = 0;
	this->busy = 0;
	
	QueueInitialise(&this->rxQueue, _receive);
	QueueInitialise(&this->txQueue, _transmit);
	
	I2CMasterInitExpClk(this->base, SysCtlClockGet(), true);
	I2CMasterEnable(this->base);
}

void IicSetSlave(i2c *this, unsigned char _slave)
{
	this->slave = _slave;
}

void IicPutData(i2c *this, unsigned char _data)
{
	QueuePush(&this->txQueue, _data);
}

void IicStartSend(i2c *this)
{
	IicStartRequest(this, 0);
}

void IicStartSendWithHook(i2c *this, i2c_callback _callback)
{
	IicStartRequestWithHook(this, 0, _callback);
}

void IicStartRequest(i2c *this, unsigned char _expected)
{
	IicStartRequestWithHook(this, _expected, NULL);
}

void IicStartRequestWithHook(i2c *this, unsigned char _expected, i2c_callback _callback)
{
	char tx;
	int  burst;
	
	if(this->busy)
		return;
	
	this->count = this->txQueue.count;
	this->busy = 1;
	
	if(this->count < 1)
		return;
	
	this->expectedResponse = _expected;
	this->callback = _callback;
	
	this->receiving = 0;
	tx = QueuePop(&this->txQueue);
	burst = (this->count > 1);
	
	I2CMasterSlaveAddrSet(this->base, this->slave, false);
	I2CMasterDataPut(this->base, tx);
	
	if(!burst)
	{
		_IicCmd(this, I2C_MASTER_CMD_SINGLE_SEND);
	}
	else
	{
		_IicCmd(this, I2C_MASTER_CMD_BURST_SEND_START);
	}
}

void IicStartReceive(i2c *this, unsigned char _amount)
{
	IicStartReceiveWithHook(this, _amount, NULL);
}

void IicStartReceiveWithHook(i2c *this, unsigned char _amount, i2c_callback _callback)
{
	int burst = (_amount > 1);
	
	if(this->busy)
		return;
	
	this->busy = 1;
	
	this->expectedResponse = _amount;
	this->callback = _callback;
	this->count = _amount;
	this->receiving = 1;
	this->rxFlag = 0;
	QueueClear(&this->rxQueue);
	
	I2CMasterSlaveAddrSet(this->base, this->slave, true);
	
	if(!burst)
	{
		_IicCmd(this, I2C_MASTER_CMD_SINGLE_RECEIVE);
	}
	else
	{
		_IicCmd(this, I2C_MASTER_CMD_BURST_RECEIVE_START);
	}
}
