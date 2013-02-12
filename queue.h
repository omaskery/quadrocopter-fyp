#ifndef QUEUE_H
#define QUEUE_H

// buffer object
typedef struct buffer_t
{
	unsigned long length;												// length of data stored at data pointer
	char *data;																	// array of data
} buffer;

typedef struct queue_t
{
	unsigned long count, capacity, front, back;	// number of elements in queue, max elements in queue, front pointer, back pointer
	char *data;																	// pointer to byte array to store elements
} queue;

// initialise a queue, takes this pointer and pointer to buffer object describing where to store data
void QueueInitialise(volatile queue*, buffer*);
// push a character to a queue, takes this* and character to store
int  QueuePush(volatile queue*, char);
// pop a character from a queue, takes this*
char QueuePop(volatile queue*);
// clears all data from a queue
void QueueClear(volatile queue*);
// getter method to query whether a queue is full, takes const this*
int  QueueIsFull(const volatile queue*);
// getter method to query whether a queue is empty, takes const this*
int  QueueIsEmpty(const volatile queue*);

#endif
