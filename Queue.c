#include "Queue.h"
#include <stdio.h>
#include <stdlib.h>
Queue q;
/* =============================================================================
 * Initialize the feilds of a Queue structure instance.
 * =============================================================================
 */
void Qinit(int n) {
	q.data = (Car**)malloc(n * sizeof(Car*));
	q.list = (Car**)malloc(n * sizeof(Car*));
	q.capacity = n;
	q.head = 0;
	q.tail = 0;
	q.count = 0;
}

/* =============================================================================
 * Free the Queue data and list arrays.
 * =============================================================================
 */
void Qfree(){
	for ( int i = 0; i < q.capacity; i++ ) {
		free(q.data[i]);
		free(q.list[i]);
	}
	free(q.data);
	free(q.list);
}

/* =============================================================================
 * Clear the Queue.
 * =============================================================================
 */
void Qclear(){
	q.head = 0;
	q.tail = 0;
	q.count = 0;
}

/* =============================================================================
 * A FIFO Queue enqueue function.
 * Check precondition Qisfull() = false.
 * =============================================================================
 */
void Qenqueue(Car *car){
	if (q.count == q.capacity) {
		printf("Queue is full\n");
		return;
	}
	q.data[q.tail] = car;
	q.list[q.tail] = car;
	q.tail = (q.tail + 1) % q.capacity;
	q.count++;
}

/* ===========================================================================
 * A FIFO Queue delete function.
 * Delete and return the car at Queue head.
 * Check precondition QisEmpty = false.
 * ===========================================================================
 */
Car* Qserve(){
	Car *car = q.data[q.head];
	q.head = (q.head + 1) % q.capacity;
	q.count--;
	return car;
}

/* ===========================================================================
 * Return the car at the head of the Queue, without deleting it.
 * ===========================================================================
 */
Car* Qpeek(){
	return q.data[q.head];
}

/* ===========================================================================
 * Return a list of the queue contents and its size.
 * ===========================================================================
 */
Car** Qiterator(int *sz){
	*sz = q.count;
	return q.list;
}

/* ===========================================================================
 * Return the capacity of the FIFO Queue.
 * ===========================================================================
 */
int Qcapacity() {
	return q.capacity;
}

/* ===========================================================================
 * Return the number of cars in the FIFO Queue.
 * ===========================================================================
 */
int Qsize(){
	return q.count;
}

/* ===========================================================================
 * Return true if the FIFO Queue is full. Return false otherwise.
 * ===========================================================================
 */
bool QisFull(){
	return (Qsize() == Qcapacity());
}

/* ===========================================================================
 * Return true if the FIFO Queue is empty. Return false otherwise.
 * ===========================================================================
 */
bool QisEmpty(){
	return (Qsize() == 0);
}