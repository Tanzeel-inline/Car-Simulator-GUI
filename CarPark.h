/*
 * CarPark.h
 *
 * Header file to provide an interface for graphical display of a CarPark
 * application. This interface requires the functions of a FIFO Queue of
 * Car-type structure. The definitions of both structures are specified
 * in the provided header files Queue.h and Car.h respectively.
 *
 * Author Dr. Abdulghani M. Al-Qasimi
 * Version 2.00 2022/02/20
 *
 */
#include "Queue.h" // Provides FIFO Queue-type structure

/* =============================================================================
 * Do not change the following constants, they provide default values and
 * definitons required by the graphical dispay.
 * =============================================================================
 */
#define IN_VALETS 5	  // Default number of in valets
#define OUT_VALETS 2  // Default number of out valets
#define VALET_LIMIT 6 // Limit the number of in or out valets
#define QUEUE_SIZE 5  // Default size of the waiting cars queue
#define QUEUE_LIMIT 8 // Limit the number of car waiting list
#define PARK_SIZE 16  // Default capacity of the parking space (minimum)
#define PARK_LIMIT 40 // Maximum capacity of the parking space (maximum)
#define EXP_CARS 5E-2 // Default expected number of in-comming cars
#define TOTAL_CARS_IMAGE 13

#define READY 1 // state of valet ready for serving cars
#define MOVE 2	// state of valet busy un/parking a car
#define FETCH 3 // state of in-valet fetching car from queue
#define WAIT 4	// state of valet waiting to access park space

/* =============================================================================
 * The following functions are defined and provided in the CarPark.o file.
 * The implemention of the car park must use them to connect correctly to
 * the graphical output.
 * =============================================================================
 * Used to set the state of the in and out valets to be displayed properly.
 * Your program shall use them where appropriate.
 * =============================================================================
 */
void setViState(int vid, int st); // Set the state of in-valet
void setVoState(int vid, int st); // Set the state of out-valet
void setViCar(int vid, Car *car); // Set the car acquired by the in-valet
void setVoCar(int vid, Car *car); // Set the car acquired by the out-valet

/* =============================================================================
 * updateStats:
 * Used to communicate statistics counters of the car park; to be shown on the
 * graphical output. Your program shall use it where appropriate.
 * Parameters:
 * oc  Number of occupied slots in the parking space
 * nc  The total number of cars created during simulation
 * pk  The total number of cars allowed to park
 * rf  The total number of cars not allowed to park
 * nm  The number of cars currently acquired by in-valets
 * sqw Accumulation sum of car-waiting times in the queue
 * spt Accumulation sum of car-parking durations
 * ut  The car-park space utilization
 * =============================================================================
 */
void updateStats(int oc, int nc, int pk, int rf, int nm, long sqw, long spt, double ut);

/* =============================================================================
 * G2DInit:
 * Initialize the graphical system; must be called at system initialization.
 * Parameters:
 *  - p   is the car park array,
 *  - psz is the park space size
 *  - nvi is the number of in-valets
 *  - nvo is the number of out-valets
 *  - plk is the lock used to protect the park
 * =============================================================================
 */
void G2DInit(Car **p, int psz, int nvi, int nvo, pthread_mutex_t plk);

/* =============================================================================
 * show:
 * Use the structures and other parameters passed to function G2DInit() to draw
 * the state of the car-park, the queue, the valets and other information on the
 * graphical output window. To animate the drawing this function must be called
 * frequently, allow at least one second delay between the calls so it can do
 * and finish one screen.
 * =============================================================================
 */
void show();

/* =============================================================================
 * finish:
 * Clean and shutdown the graphical system
 * =============================================================================
 */
void finish();

/* =============================================================================
 * newCars:
 * Return the number of new cars to create at each loop cycle
 * using a random number with Poisson distribution according
 * to the expected number passed to it in the parameter
 * =============================================================================
 */
int newCars(double ex);
