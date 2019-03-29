/*
 * ultrasonic.h
 *
 *  Created on: Mar 23, 2019
 *      Author: michelle
 */

#include <stdbool.h>

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#define LEFT        (0)     // Left sensor
#define MIDDLE      (1)     // Middle sensor
#define RIGHT       (2)     // Right sensor

int sensor;                 // Determines which sensor is being used
int prev_sensor;            // Keeps track of previous sensor
int num_of_samples;         // Counts # of samples taken per 100
float sum;                  // Sum of distances to calculate average distance
float dist;
unsigned int up_counter;    // Temp variable to store counter
int bound;
double samples;
bool speakL;
bool speakM;
bool speakR;

extern void ultra_init(void);
extern void ultra_sensors(void);
extern void checkBoundaries(void);
extern void average(void);
void clockSystemReset(void);


#endif /* ULTRASONIC_H_ */
