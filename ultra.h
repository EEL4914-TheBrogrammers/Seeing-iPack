/*
 * ultra.h
 *
 *  Created on: Apr 11, 2019
 *      Author: michelle
 */

#ifndef ULTRA_H_
#define ULTRA_H_
#define LEFT    (0x01)
#define RIGHT   (0x02)

float distance;
int int_count;
float rising;
float falling;
int side;

float find_distance(void);
void initUltra(int sensor);


#endif /* ULTRA_H_ */
