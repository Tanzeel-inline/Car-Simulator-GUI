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
sem_t inVal_sem;
sem_t outVal_sem;

//Queue initialization
Queue q;

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
	return NULL;
}

void *outValet(void *arg) {
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

	//Initialize the car park
	Car **p = (Car**)malloc(psize * sizeof(Car*));


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
				Qenqueue(car);
			}
			printCar(Qpeek());
			pthread_mutex_unlock(&queue_opr_mutex);
			allowed_cars++;
		}
		refused_cars += (carsToSpawn - i);

		while ( occupied_slots < psize && !QisEmpty() ) {
			Car *car = Qserve();
			car->ptm = timer;
			printf("Serving car: %d\n", car->cid);
			printCar(car);
			occupied_slots++;
			show();
		}
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