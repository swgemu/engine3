/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "Math.h"

double Math::PI = M_PI;
double Math::DEG2RAD = M_PI / 180.0f;
double Math::RAD2DEG = 180.0f / M_PI;
double Math::E = 2.71828182845904523536;

double Math::sqrt(double number) { // from Q3 fix for 64 bit
	/*long i;
	float x, y;
	const float f = 1.5F;

	x = number * 0.5F;
	y  = number;

	i  = * (long*) &y;
	i  = 0x5f3759df - ( i >> 1 );

	y  = * (float*) &i;
	y  = y * (f - (x * y * y));
	y  = y * (f - (x * y * y));
	return number * y;*/
	return ::sqrt(number);
}

/*float Math::rsqrt(float number) {
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * (long*) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * (float*) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );
	y  = y * ( threehalfs - ( x2 * y * y ) );

	return y;
}

float Math::fabs(float f) {
	int tmp = *(int*)&f;
	tmp &= 0x7FFFFFFF;
	return *(float*) &tmp;
}*/
