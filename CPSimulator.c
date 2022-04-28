#include "Car.h"
#include "CarPark.h"
#include "Queue.h"
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>


//Global variables
int car_id = 0;
int psize = 16, inval = 3, outval = 2, qsize = 8;
float expnum = 1.50;
long timer = 0;

int total_cars = 0;
int refused_cars = 0;
int allowed_cars = 0;
int occupied_slots = 0;
int cars_acquired_by_invalet = 0;
long sum_waiting_time = 0;
long sum_car_parking = 0;
int waiting_car_count = 0;
int car_parking_count = 0;


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
pthread_t mon;

//Valet id's
int *inValsID;
int *outValsID;


//Time variables
time_t start_simulation;
time_t stop_simulation;


//OutValet queue store
Car **outValetStore;
int OVSHead = 0;
int OVSTail = 0;
int OVSCount = 0;

char *getTime() {
	time_t tempTime;
	time(&tempTime);
	char *time_str = ctime(&tempTime);
	time_str[strlen(time_str) - 1] = '\0';
	return time_str;
}

long getCurrentTime() {
	time_t tempTime;
	time(&tempTime);

	return tempTime;
}

void Car_Init(Car *p) {
	p->cid = car_id++;
	p->vid = (rand() % inval) + 1;
	p->sno = (rand() % psize);
	p->atm = getCurrentTime();
	p->ltm = 20;
	sprintf(p->pn, ".cars/Car%d.bmp", p->cid);
	sprintf(p->pnf, ".cars/Car%df.bmp", p->cid);
	car_id = car_id % TOTAL_CARS_IMAGE; 
}

void printCar(Car *p) {
	printf("Car %d: %d %d %ld %ld %s %s\n", p->cid, p->vid, p->sno, p->atm, p->ltm, p->pn, p->pnf);
}

void ctrlCHandle(int sig) {

	time(&stop_simulation);

	printf("\n\n\n%s:\tRecevied shutdown signal\n", getTime());
	printf("%s:\tCar park valet is shutting down\n", getTime());
	printf("%s:\tThe valets are leaving\n", getTime());
	for ( int i = 0 ; i < inval; i++ ) {
		pthread_cancel(inValThreads[i]);
	}	

	for ( int i = 0 ; i < outval; i++ ) {
		pthread_cancel(outValThreads[i]);
	}

	printf("%s:\tDone. %d valets left.\n", getTime(), inval + outval);

	//finish();

	printf("%s:\tMonitor exiting\n\n\n", getTime());


	sem_destroy(&inVal_sem);
	sem_destroy(&outVal_sem);
	sem_destroy(&inVal_parking_sem);
	pthread_mutex_destroy(&queue_opr_mutex);
	pthread_mutex_destroy(&parking_opr_mutex);

	char *time_str = ctime(&start_simulation);
	time_str[strlen(time_str) - 1] = '\0';
	printf("Simulator started at:\t%s\n", time_str);
	printf(" Parking capacity was:\t%d\n", psize);
	printf(" Allowed queue length was:\t%d\n", Qcapacity());
	printf(" Number of in valets was:\t%d\n", inval);
	printf(" Number of out valets was:\t%d\n", outval);
	printf(" Expected arrivals was:\t%.2f\n", expnum);
	time_str = ctime(&stop_simulation);
	time_str[strlen(time_str) - 1] = '\0';
	printf("Simulator stopped at:\t%s\n\n\n", time_str);
	free(inValThreads);
	free(outValThreads);
	free(inValsID);
	free(outValsID);
	//Qfree(q);

	for ( int i = 0 ; i < car_counter; i++ ) {
		free(p[i]);
	}
	free(p);
	
	printf("CP Simulation was executed for:\t%ld\n",stop_simulation - start_simulation);
	printf("Total number of car processed:\t%d\n", allowed_cars);
	printf("  Number of car that parked:\t%d\n", waiting_car_count);
	printf("  Number of car that turned away:\t%d\n", refused_cars);
	printf("  Number of car in transit\t%d\n", OVSCount);
	printf("  Number of cars still queued:\t%d\n",Qsize());
	printf("  Number of cars still parked:\t%d\n\n\n",psize);

	printf("Average queue waiting time:\t%.2f\n", sum_waiting_time * 1.0 / waiting_car_count );
	printf("Average parking time:\t%.2f\n\n\n", sum_car_parking * 1.0 / car_parking_count );

	printf("%s:\tCarpark exits.\n", getTime());
	//finish();
	exit(0);
}

void SleepFloat(float sec) {
    struct timespec ts;
    ts.tv_sec = (int) sec;
    ts.tv_nsec = (sec - ((int) sec)) * 1000000000;
    nanosleep(&ts,NULL);
}

void *inValet(void *arg) {

	int id = *(int *)arg;
	//int id = 1;
	printf("Invalet loop %d\n", id);
	while ( true ) {
		
		Car *car;
		//setViState(id, READY);
		sem_wait(&inVal_sem);
		//setViState(id, FETCH);
		pthread_mutex_lock(&queue_opr_mutex);
		if ( QisEmpty() ) {
			SleepFloat(0.2);
			pthread_mutex_unlock(&queue_opr_mutex);
			continue;
		}
		else {
			printf("Invalet %d: Fetching car from queue\n", id);
			car = Qserve();
			cars_acquired_by_invalet++;
			//setViState(id, WAIT);
			//setViCar(id, car);
		}
		SleepFloat(0.2);
		pthread_mutex_unlock(&queue_opr_mutex);

		sem_wait(&inVal_parking_sem);

		pthread_mutex_lock(&parking_opr_mutex);
		if ( car_counter == psize ) {
			SleepFloat(0.2);
			pthread_mutex_unlock(&parking_opr_mutex);
			continue;
		}
		else {
			printf("Invalet %d: Parking car\n", id);
			//Sleep before parking the car
			sleep(1);
			p[tail_val] = car;
			p[tail_val]->ptm = getCurrentTime();
			p[tail_val]->sno = tail_val;
			sum_waiting_time += (p[tail_val]->ptm - p[tail_val]->atm);
			waiting_car_count++;
			tail_val++;
			car_counter++;
			cars_acquired_by_invalet--;
			car->vid = id;
			//setViState(id, MOVE);
			//Sleep after parking the car
			sleep(1);
		}
		SleepFloat(0.2);
		pthread_mutex_unlock(&parking_opr_mutex);
	}
	
	return NULL;
}

void *outValet(void *arg) {

	int id = *(int *)arg;
	//int id = 1;
	while ( true ) {
		Car *car;
		//setVoState(id, READY);
		sem_wait(&outVal_sem);
		//Valet is waiting to access park space, to unpark the car
		//setVoState(id, WAIT);
		pthread_mutex_lock(&parking_opr_mutex);
		//There is no car in the parking lot, so valet is going back to ready state
		if ( OVSCount == 0 ) {
			SleepFloat(0.2);
			pthread_mutex_unlock(&parking_opr_mutex);
			continue;
		}
		else {
			//Valet is going to serve the car
			//setVoState(id, MOVE);
			//setVoCar(id, car);
			//Looking for car that is ready to be unparked
			sleep(1);
			car = outValetStore[OVSHead];
			OVSHead++;
			OVSCount--;
			sleep(1);
		}
		//Sleep for 0.2s before exiting critical section
		SleepFloat(0.2);
		pthread_mutex_unlock(&parking_opr_mutex);
		
	}
	
	return NULL;
}

void *monitor(void *arg) {

	while ( true ) {
		updateStats(tail_val, total_cars, allowed_cars, refused_cars, cars_acquired_by_invalet, sum_waiting_time, sum_car_parking, (tail_val * 100.0) / psize);
		show();
		sleep(1);	
	}
	//updateStats()
	return NULL;
}


int main(int argc, char *argv[]) {
	//Random seed
	srand(time(NULL));

	//Control c signal handle function
	signal(SIGINT, ctrlCHandle);

	time(&start_simulation);

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
	inValsID = (int *) malloc(sizeof(int) * inval);
	outValsID = (int *) malloc(sizeof(int) * outval);

	//Initialize the car park
	p = (Car**)malloc(psize * sizeof(Car*));
	for ( int i =- 0 ; i < psize ; i++ ) {
		p[i] = NULL;
	}

	//Initialize the car queue
	Qinit(qsize);

	//Queue for outvalet
	outValetStore = (Car**)malloc(outval * sizeof(Car*));
	for ( int i = 0 ; i < outval ; i++ ) {
		outValetStore[i] = NULL;
	}

	//Thread creation
	for ( int i = 0 ; i < inval ; i++ ) {
		printf("[MAIN]:InValet thread %d created\n", i);
		inValsID[i] = i + 1;
		pthread_create(&inValThreads[i], NULL, inValet,&inValsID[i]);
	}

	for ( int i = 0 ; i < outval ; i++ ) {
		printf("[MAIN]:OutValet thread %d created\n", i);
		outValsID[i] = i + 1;
		pthread_create(&outValThreads[i], NULL, outValet,&outValsID[i]);
	}


	//Initializing the car display
	G2DInit(p, psize, inval, outval, lock);
	show();

	pthread_create(&mon, NULL, monitor, NULL);
	//Infinite main thread loop
	while ( true ) {
		//Poisson distribution
		int carsToSpawn = newCars(expnum);
		printf("[MAIN]:%d cars to spawn\n", carsToSpawn);

		//This part deals with inValet operations
		//If there is space in the queue, spawn cars
		int i = 0;
		for ( ; i < carsToSpawn ; i++ ) {
			//printCar(car);
			pthread_mutex_lock(&queue_opr_mutex);
			total_cars += carsToSpawn;
			//printf("[MAIN]:Queue locked gained by Main thread\n");
			if ( QisFull() ) {
				printf("[MAIN]:Queue is full\n");
				//printf("[MAIN]:Queue locked released by Main thread\n");
				refused_cars += (carsToSpawn - i);
				pthread_mutex_unlock(&queue_opr_mutex);
				break;
			}
			else {
				printf("[MAIN]:Spawning car: %d\n", i);
				//Create the car
				Car *car = (Car*)malloc(sizeof(Car));
				CarInit(car);
				Car_Init(car);
				//Enqueue the car and increment the invalet semaphore
				Qenqueue(car);
				sem_post(&inVal_sem);
			}
			printCar(Qpeek());
			printf("[MAIN]:Qsize is : %d\n", Qsize());
			//printf("[MAIN]:Queue locked released by Main thread\n");
			allowed_cars++;
			pthread_mutex_unlock(&queue_opr_mutex);
		}

		//This part deals with outValet operation
		pthread_mutex_lock(&parking_opr_mutex);
		long currTime = getCurrentTime();
		for ( int i = 0 ; i < tail_val ; i++ ) {
			//No space in queue of outvalet
			if ( OVSCount >= outval ) {
				break;
			}
			printf("%ld - %ld\n", p[i]->ptm + p[i]->ltm, currTime);
			if ( p[i]->ptm + p[i]->ltm <= currTime ) {
				outValetStore[OVSTail] = p[i];
				sum_car_parking += (outValetStore[OVSTail]->ltm);
				OVSTail = (OVSTail + 1) % outval;
				OVSCount++;
				
				for ( int j = i ; j < tail_val - 1 ; j++ ) {
					p[j] = p[j+1];
				}
				p[tail_val - 1] = NULL;
				tail_val--;
				car_counter--;
				car_parking_count++;

				sem_post(&inVal_parking_sem);
				sem_post(&outVal_sem);
			}
		}
		pthread_mutex_unlock(&parking_opr_mutex);
		//Sleep for 1 second
		sleep(1);
		//Our timer
		timer++;
		printf("[MAIN]:Parking lot slots occupied: %d\n", tail_val);
	}
	return 0;
}