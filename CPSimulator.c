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
long timer = 0;

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

//Threads variable
pthread_t *inValThreads;
pthread_t *outValThreads;


void Car_Init(Car *p) {
	p->cid = car_id++;
	p->vid = (rand() % inval) + 1;
	p->sno = (rand() % psize);
	p->atm = timer;
	p->ltm = 5;
	sprintf(p->pn, "Car%d.bmp", p->cid);
	sprintf(p->pnf, "Car%df.bmp", p->cid);
	car_id = car_id % TOTAL_CARS_IMAGE; 
}

void printCar(Car *p) {
	printf("Car %d: %d %d %ld %ld %s %s\n", p->cid, p->vid, p->sno, p->atm, p->ltm, p->pn, p->pnf);
}

void ctrlCHandle(int sig) {

	for ( int i = 0 ; i < inval; i++ ) {
		pthread_cancel(inValThreads[i]);
	}

	// for ( int i = 0 ; i < outval; i++ ) {
	// 	pthread_cancel(outValThreads[i]);
	// }

	sem_destroy(&inVal_sem);
	sem_destroy(&outVal_sem);
	sem_destroy(&inVal_parking_sem);
	pthread_mutex_destroy(&queue_opr_mutex);
	pthread_mutex_destroy(&parking_opr_mutex);

	finish();
	exit(0);
}

void SleepFloat(float sec) {
    struct timespec ts;
    ts.tv_sec = (int) sec;
    ts.tv_nsec = (sec - ((int) sec)) * 1000000000;
    nanosleep(&ts,NULL);
}

void *inValet(void *arg) {
	//One will wait for queue addition
	sem_wait(&inVal_sem);
	printf("[INVALET]: Car added in queue, inValet signalled\n");
	//One will wait for parking empty space
	sem_wait(&inVal_parking_sem);
	printf("[INVALET]: Space availible in parking, inValet signalled\n");
	pthread_mutex_lock(&queue_opr_mutex);
	printf("[INVALET]: Queue locked gained by inValet\n");
	if ( QisEmpty() ) {
		//Since, we have no car to park
		sem_post(&inVal_parking_sem);
	}
	else {
		//Pick the car, wait for .2 seconds
		Car *holder = Qserve();
		printf("[INVALET]: Car %d picked from queue with size %d, inValet signalled\n", holder->cid, Qsize());
		//Send the car to parking lot
		p[tail_val] = holder;
		p[tail_val]->ptm = timer;
		tail_val++;
		car_counter++;
		//sleep(0.2);
		SleepFloat(0.2);
	}
	printf("[INVALET]: Queue locked released by inValet\n");
	pthread_mutex_unlock(&queue_opr_mutex);
	return NULL;
}

void *outValet(void *arg) {

	sem_wait(&outVal_sem);

	pthread_mutex_lock(&parking_opr_mutex);
	printf("Parking lot locked gained by outValet\n");
	printf("------------\n");
	for ( int i = 0 ; i < tail_val ; i++ ) {
		if ( p[i]->ptm + p[i]->ltm <= timer ) {
			Car *outValHolder = p[i];
			printf("Car %d outValeted\n", outValHolder->cid);
			//Remove the car from parking lot
			for ( int j = i ; j < tail_val - 1 ; j++ ) {
				p[j] = p[j+1];
			}
			tail_val--;
			car_counter--;
			sem_post(&inVal_parking_sem);
			//sleep(0.2)
			SleepFloat(0.2);
		}
	}
	printf("------------\n");
	printf("Parking lot locked released by outValet\n");
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

	int sem_size;
	sem_getvalue(&inVal_parking_sem, &sem_size);
	printf("[MAIN]:Car park size: %d\n", sem_size);
	//Initialize the threads variable
	inValThreads = (pthread_t *) malloc(sizeof(pthread_t) * inval);
	outValThreads = (pthread_t *) malloc(sizeof(pthread_t) * outval);

	//Initialize the car park
	p = (Car**)malloc(psize * sizeof(Car*));

	//Initialize the car queue
	Qinit(qsize);

	//Initializing the car display
	//G2DInit(p, psize, inval, outval, lock);
	//show();

	for ( int i = 0 ; i < inval ; i++ ) {
		pthread_create(&inValThreads[i], NULL, inValet, NULL);
	}

	// for ( int i = 0 ; i < outval ; i++ ) {
	// 	pthread_create(&outValThreads[i], NULL, outValet, NULL);
	// }


	//Infinite main thread loop
	while ( true ) {
		//Poisson distribution
		int carsToSpawn = newCars(expnum);
		printf("[MAIN]:%d cars to spawn\n", carsToSpawn);
		//If there is space in the queue, spawn cars
		int i = 0;
		for ( ; i < carsToSpawn ; i++ ) {
			printf("[MAIN]:Spawning car: %d\n", i);
			Car *car = (Car*)malloc(sizeof(Car));
			CarInit(car);
			Car_Init(car);
			//printCar(car);
			pthread_mutex_lock(&queue_opr_mutex);
			printf("[MAIN]:Queue locked gained by Main thread\n");
			if ( QisFull() ) {
				printf("[MAIN]:Queue is full\n");
				printf("[MAIN]:Queue locked released by Main thread\n");
				pthread_mutex_unlock(&queue_opr_mutex);
				break;
			}
			else {
				//Enqueue the car and increment the invalet semaphore
				Qenqueue(car);
				sem_post(&inVal_sem);
			}
			printCar(Qpeek());
			printf("[MAIN]:Qsize is : %d\n", Qsize());
			printf("[MAIN]:Queue locked released by Main thread\n");
			pthread_mutex_unlock(&queue_opr_mutex);
			allowed_cars++;
		}
		refused_cars += (carsToSpawn - i);

		pthread_mutex_lock(&parking_opr_mutex);
		printf("[MAIN]:Parking locked gained by Main thread\n");
		for ( int i = 0 ; i < tail_val ; i++ ) {
			printf("%ld - %ld\n",p[i]->ptm + p[i]->ltm , timer);
			if ( p[i]->ptm + p[i]->ltm == timer ) {
				printf("[MAIN]:Car ready to be removed\n");
				//Tell the out valet that some car is ready to be unparked
				sem_post(&outVal_sem);
			}
		}
		printf("[MAIN]:Parking locked released by Main thread\n");
		pthread_mutex_unlock(&parking_opr_mutex);
		//Sleep for 1 second
		sleep(1);
		//Our timer
		timer++;
		printf("[MAIN]:Parking lot: %d\n", tail_val);
	}
	// updateStats(psize, psize, 10, 1, 3, 10, 10, 100);
	// show();
	// sleep(4);
	// updateStats(psize, psize, 10, 1, 3, 20, 20, 100);
	// show();
	// sleep(4);
	// finish();
	return 0;
}