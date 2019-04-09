#include <stdbool.h>

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#define LEFT        (0)     // Left sensor
#define MIDDLE      (1)     // Middle sensor
#define RIGHT       (2)     // Right sensor

int sent;
int sensor;                 // Determines which sensor is being used
int prev_sensor;            // Keeps track of previous sensor
int num_of_samples;         // Counts # of samples taken per 100
float sum;                  // Sum of distances to calculate average distance
float dist;
unsigned int up_counter;    // Temp variable to store counter
int bound;
int side_bound;
double samples;
bool speakL;
bool speakM;
bool speakR;
float left_val;
float right_val;
float middle_val;
int counttt;

extern void ultra_init(void);
extern void ultra_sensors(void);
extern void checkBoundaries(void);
extern void average(void);
void clockSystemReset(void);


#endif /* ULTRASONIC_H_ */
