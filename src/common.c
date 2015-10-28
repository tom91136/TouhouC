#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846

// http://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-from-within-a-range
unsigned int randRange(unsigned int min, unsigned int max) {
	int r;
	const unsigned int range = 1 + max - min;
	const unsigned int buckets = RAND_MAX / range;
	const unsigned int limit = buckets * range;

	/* Create equal size buckets all in a row, then fire randomly towards
	 * the buckets until you land in one of them. All buckets are equally
	 * likely. If you land off the end of the line of buckets, try again. */
	do {
		r = rand();
	} while (r >= limit);

	return min + (r / buckets);
}

// modified from http://stackoverflow.com/questions/5294955/how-to-scale-down-a-range-of-numbers-with-a-known-min-and-max-value
// for convenience, I've written this before
double scale(double valueIn, double baseMin, double baseMax, double limitMin,
		double limitMax) {
	return ((limitMax - limitMin) * (valueIn - baseMin) / (baseMax - baseMin))
			+ limitMin;
}

double wrapDegree(double input) {
	return input > 360 ? input - 360 : input;
}

double toDeg(double radians) {
	return radians * (180.0 / M_PI);
}

double toRad(double deg) {
	return deg * (M_PI / 180.0);
}

bool isInteger(char *input) {
	int i;
	for (i = 0; i < strlen(input); i++) {
		if (!isdigit(input[i]))
			return false;
	}
	return true;
}
