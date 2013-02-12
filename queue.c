#include "queue.h"

void QueueInitialise(volatile queue *this, buffer *_storage)
{
	this->capacity = _storage->length;
	this->count = 0;
	this->front = 0;
	this->back = 0;
	this->data = _storage->data;
}

int  QueuePush(volatile queue *this, char _data)
{
	if(QueueIsFull(this))
		return 0;
	
	this->data[this->back] = _data;
	this->back = (this->back + 1) % this->capacity;
	this->count ++;
	
	return 1;
}

char QueuePop(volatile queue *this)
{
	char result = '\0';
	
	if(!QueueIsEmpty(this))
	{
		result = this->data[this->front];
		this->front = (this->front + 1) % this->capacity;
		this->count --;
	}
	
	return result;
}

void QueueClear(volatile queue *this)
{
	this->front = 0;
	this->back = 0;
	this->count = 0;
}

int  QueueIsFull(const volatile queue *this)
{
	return (this->count >= this->capacity);
}

int  QueueIsEmpty(const volatile queue *this)
{
	return (this->count <= 0);
}
