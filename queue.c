#include "queue.h"

void QueueInitialise(queue *this, buffer *_storage)
{
	this->capacity = _storage->length;
	this->count = 0;
	this->front = 0;
	this->back = 0;
	this->data = _storage->data;
}

int  QueuePush(queue *this, char _data)
{
	if(QueueIsFull(this))
		return 0;
	
	this->data[this->back] = _data;
	this->back = (this->back + 1) % this->capacity;
	this->count ++;
	
	return 1;
}

char QueuePop(queue *this)
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

int  QueueIsFull(const queue *this)
{
	return (this->count >= this->capacity);
}

int  QueueIsEmpty(const queue *this)
{
	return (this->count <= 0);
}
