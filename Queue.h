/*
 * Queue.h
 *
 * Header file for a FIFO queue structure using an array of
 * Car-type elements stored in it.
 *
 * The following function specifications shall be used to
 * implement the FIFO queue:
 *
 * - void  Qinit(int n)
 * - void  Qfree()
 * - void  Qclear()
 * - void  Qenqueue(Car *car)
 * - Car*  Qserve()
 * - Car*  Qpeek()
 * - Car** Qiterator(int *sz)
 * - int   Qcapacity()
 * - int   Qsize()
 * - bool  QisFull()
 * - bool  QisEmpty()
 *
 * Author Dr. Abdulghani M. Al-Qasimi
 * Version 2.00 2022/02/20
 *
 */
#ifndef QUEUE_H_
#define QUEUE_H_

#include <pthread.h>		// Provides thread-safe mutex lock
#include <stdbool.h>        // Provides boolean data type
#include "Car.h"			// Provides the required Car type structure

/* =============================================================================
 * FIFO Queue struct holds the queue array and its standard field variables.
 * To implement this queue write the standard queue operations with signatures
 * as listed below. [Qinit(), Qfree(), Qenqueue(), Qserve(), Qpeek(), Qsize(),
 * Qcapacity(), Qclear(), QisEmpty(), and QisFull()].
 *
 * Also use the extra field "list" to write function [Qiterator] that returns
 * a list used to traverse all the elements of the queue starting at the head
 * and ending at the tail without changing the state of the queue.
 * =============================================================================
 */
typedef struct Queue_t {
    Car **data;       		// Array to hold car queue
	Car **list;				// Array to hold car list
    int capacity;      		// The array (queue) capacity
    int count;              // Number of cars currently in the queue
    int tail;             	// The queue tail position
    int head;            	// The queue head position
} Queue;

/* =============================================================================
 * Initialize the feilds of a Queue structure instance.
 * =============================================================================
 */
void Qinit(int n);

/* =============================================================================
 * Free the Queue data and list arrays.
 * =============================================================================
 */
void Qfree();

/* =============================================================================
 * Clear the Queue.
 * =============================================================================
 */
void Qclear();

/* =============================================================================
 * A FIFO Queue enqueue function.
 * Check precondition Qisfull() = false.
 * =============================================================================
 */
void Qenqueue(Car *car);

/* ===========================================================================
 * A FIFO Queue delete function.
 * Delete and return the car at Queue head.
 * Check precondition QisEmpty = false.
 * ===========================================================================
 */
Car* Qserve();

/* ===========================================================================
 * Return the car at the head of the Queue, without deleting it.
 * ===========================================================================
 */
Car* Qpeek();

/* ===========================================================================
 * Return a list of the queue contents and its size.
 * ===========================================================================
 */
Car** Qiterator(int *sz);

/* ===========================================================================
 * Return the capacity of the FIFO Queue.
 * ===========================================================================
 */
int Qcapacity();

/* ===========================================================================
 * Return the number of cars in the FIFO Queue.
 * ===========================================================================
 */
int Qsize();

/* ===========================================================================
 * Return true if the FIFO Queue is full. Return false otherwise.
 * ===========================================================================
 */
bool QisFull();

/* ===========================================================================
 * Return true if the FIFO Queue is empty. Return false otherwise.
 * ===========================================================================
 */
bool QisEmpty();

#endif /* QUEUE_H_ */