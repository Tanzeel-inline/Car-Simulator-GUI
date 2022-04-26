/*
 * Car.h
 *
 * Header file for a Car type structure for use in a CarPark.
 * It is used to store the car ID, time of arrival, time of parking,
 * expected time to leave, in-valet-id, parking slot number and the
 * car's image file name. fields cid, atm, ltm, pn and pnf are set
 * by calling CarInit() function at car creation time. vid must be
 * set when the car is picked from the queue by a particular valet.
 * When the car is parked at a prticular slot, sno and ptm must be
 * set and ltm must be updated. Field ltm can be used by out-valets
 * as a priority value for selecting which car is leaving the park.
 * In order to use this structure in the CarPark application, the
 * needed function with its signature given below as specified.
 *
 * Author Dr. Abdulghani M. Al-Qasimi
 * Version 2.00 2022/02/20
 *
 */
#ifndef CAR_H_
#define CAR_H_
// Include library having time_t type
#include <time.h>
typedef struct Car_t
{
	int cid;	  // The car's unique ID
	int vid;	  // The in-valet's ID
	int sno;	  // The parking slot number
	time_t atm;	  // The time of arrival (creation)
	time_t ptm;	  // The time of parking (start time)
	time_t ltm;	  // The expected time to leave (end time)
	char pn[20];  // The car's image file name
	char pnf[20]; // The car's flipped-image file name
} Car;

/* =============================================================================
 * CarInit:
 * Initialize the newly created car passed in the parameter. Before using a new
 * car it must be initialized by a call to this function which is implemented
 * in the CarPark.o file.
 * Note: This function does not create a car object.
 * =============================================================================
 * Initialize the newly created car by setting the following fields:
 *  - cid is set to a unique car ID.
 *  - sno is set to 0.
 *  - atm is set to the current time -- time of creation.
 *  - ltm is set to a random time limited to 180 seconds -- time to stay parked.
 *  - pn  is set to the file name of a random car image limited to 13.
 *  - pnf is set to the file name of the horizontaly flipped same image as pn.
 * =============================================================================
 */
void CarInit(Car *car);
#endif