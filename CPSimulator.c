#include "Car.h"
#include "CarPark.h"
#include "Queue.h"
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>


//Global variables
int car_id = 0;
int psize = 40, inval = 3, outval = 2, qsize = 8, expnum = 1.50;
int timer = 0;

int refused_cars = 0;
int allowed_cars = 0;
int occupied_slots = 0;

//Deadlock and starvation operations
pthread_mutex_t queue_opr_mutex;
pthread_mutex_t parking_opr_mutex;
sem_t inVal_sem;
sem_t outVal_sem;
sem_t inVal_parking_sem;

//Queue initialization
Queue q;

//CAR PARKING INITIALIATION
Car **p;
int car_counter = 0;
int head_val = 0;
int tail_val = 0;

void Car_Init(Car *p) {
	p->cid = car_id++;
	p->vid = (rand() % inval) + 1;
	p->sno = (rand() % psize);
	p->atm = timer;
	sprintf(p->pn, "Car%d.bmp", p->cid);
	sprintf(p->pnf, "Car%df.bmp", p->cid);
	car_id = car_id % TOTAL_CARS_IMAGE; 
}

void printCar(Car *p) {
	printf("Car %d: %d %d %ld %ld %s %s\n", p->cid, p->vid, p->sno, p->atm, p->ltm, p->pn, p->pnf);
}

void ctrlCHandle(int sig) {
	finish();
	exit(0);
}
void *inValet(void *arg) {
	//One will wait for queue addition
	sem_wait(&inVal_sem);
	//One will wait for parking empty space
	sem_wait(&inVal_parking_sem);
	//
	pthread_mutex_lock(&queue_opr_mutex);
	if ( QisEmpty() ) {
		//Since, we have no car to park
		sem_post(&inVal_parking_sem);
	}
	else {
		//Pick the car, wait for .2 seconds
		Car *holder = Qserve();
		//sleep(0.2);
		//Send the car to parking lot
		p[head_val++] = holder;
		//Reset the counter if it's at the end
		if ( head_val >= psize ) {
			head_val = 0;
		}
		car_counter++;
	}
	
	pthread_mutex_unlock(&queue_opr_mutex);
	return NULL;
}

void *outValet(void *arg) {

	sem_wait(&outVal_sem);

	pthread_mutex_lock(&parking_opr_mutex);

	pthread_mutex_unlock(&parking_opr_mutex);
	return NULL;
}

int main(int argc, char *argv[]) {
	//Random seed
	srand(time(NULL));

	//Control c signal handle function
	signal(SIGINT, ctrlCHandle);
	//Command line arguments
	if ( argc > 1) {
		psize = atoi(argv[1]);
	}
	else if ( argc > 2 ) {
		inval = atoi(argv[2]);
	}
	else if ( argc > 3 ) {
		outval = atoi(argv[3]);
	}
	else if ( argc > 4 ) {
		qsize = atoi(argv[4]);
	}
	else if ( argc > 5 ) {
		expnum = atoi(argv[5]);
	}

	//Mutex lock to protect the car park
	pthread_mutex_t lock;

	//Locks initialization
	pthread_mutex_init(&lock, NULL);
	pthread_mutex_init(&queue_opr_mutex, NULL);
	pthread_mutex_init(&parking_opr_mutex, NULL);
	sem_init(&inVal_sem, 0, 0);
	sem_init(&outVal_sem, 0, 0);
	sem_init(&inVal_parking_sem, 0, psize);

	//Initialize the car park
	p = (Car**)malloc(psize * sizeof(Car*));


	// for ( int i = 0 ; i < psize; i++ ) {
	// 	p[i] = (Car*)malloc(sizeof(Car));
	// 	CarInit(p[i]);
	// 	Car_Init(p[i]);
	// 	printCar(p[i]);
	// 	//printf("%s\n",p[i]->pn);
	// }

	//Initialize the car queue
	Qinit(qsize);

	//Initializing the car display
	G2DInit(p, psize, inval, outval, lock);
	show();

	//Infinite main thread loop
	while ( true ) {
		//Poisson distribution
		int carsToSpawn = newCars(expnum);
		printf("%d cars to spawn\n", carsToSpawn);
		//If there is space in the queue, spawn cars
		int i = 0;
		for ( ; i < carsToSpawn ; i++ ) {
			printf("Spawning car: %d\n", i);
			Car *car = (Car*)malloc(sizeof(Car));
			CarInit(car);
			Car_Init(car);
			printCar(car);
			pthread_mutex_lock(&queue_opr_mutex);
			if ( Qisfull() ) {
				printf("Queue is full\n");
				break;
			}
			else {
				//Enqueue the car and increment the invalet semaphore
				Qenqueue(car);
				sem_post(&inVal_sem);
			}
			printCar(Qpeek());
			pthread_mutex_unlock(&queue_opr_mutex);
			allowed_cars++;
		}
		refused_cars += (carsToSpawn - i);


		//This is useless code
		while ( occupied_slots < psize && !QisEmpty() ) {
			Car *car = Qserve();
			car->ptm = timer;
			printf("Serving car: %d\n", car->cid);
			printCar(car);
			occupied_slots++;
			show();
		}

		// pthread_mutex_lock(&parking_opr_mutex);
		
		// for ( )
		// pthread_mutex_unlock(&parking_opr_mutex);
		//Sleep for 1 second
		sleep(1);
		//Our timer
		timer++;
	}
	updateStats(psize, psize, 10, 1, 3, 10, 10, 100);
	show();
	sleep(4);
	updateStats(psize, psize, 10, 1, 3, 20, 20, 100);
	show();
	sleep(4);
	finish();
	return 0;
}