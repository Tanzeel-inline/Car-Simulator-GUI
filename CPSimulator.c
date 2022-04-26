#include "Car.h"
#include "CarPark.h"
#include "Queue.h"
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int car_id = 0;
int psize = 40, inval = 3, outval = 2, qsize = 8, expnum = 1.50;
int timer = 0;


void Car_Init(Car *p) {
	p->cid = car_id++;
	p->vid = (rand() % inval) + 1;
	p->sno = (rand() % psize) + 1;
	p->atm = timer;
	sprintf(p->pn, "Car%d.bmp", p->cid);
	sprintf(p->pnf, "Car%df.bmp", p->cid);
	car_id = car_id % 13; 
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
	pthread_mutex_init(&lock, NULL);

	//Initialize the car park
	Car **p = (Car**)malloc(psize * sizeof(Car*));
	for ( int i = 0 ; i < psize; i++ ) {
		p[i] = (Car*)malloc(sizeof(Car));
		CarInit(p[i]);
		Car_Init(p[i]);
		printCar(p[i]);
		//printf("%s\n",p[i]->pn);
	}

	//Initialize the car queue
	Queue q;
	Qinit(qsize);

	//Display the car park
	G2DInit(p, psize, inval, outval, lock);
	//show();
	while ( true ) {
		int carsToSpawn = newCars(expnum);
		printf("%d cars to spawn\n", carsToSpawn);
		sleep(1);
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