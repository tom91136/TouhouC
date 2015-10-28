/*
 * common.h
 *
 *  Created on: Sep 29, 2015
 *      Author: Tom
 */

#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

#include <stdbool.h>



unsigned int randRange(unsigned int min, unsigned int max);

 double wrapDegree(double input);

 double toDeg(double radians);

 double toRad(double deg);

double scale(double valueIn, double baseMin, double baseMax, double limitMin,
		double limitMax);

bool isInteger(char *input);

#endif /* SRC_COMMON_H_ */
