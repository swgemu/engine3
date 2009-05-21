/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MATH_
#define MATH_

#include <math.h>

namespace sys {
  namespace lang {

	class Math {
	public:
		static const double PI = M_PI;
		static const double DEG2RAD = M_PI / 180.0f;
		static const double RAD2DEG = 180.0f / M_PI;

		static float sqrt(float number);

		static float rsqrt(float number);

		static float fabs(float f);

		static double acos(double arg) {
			return ::acos(arg);
		}

		static double sin(double arg) {
			return ::sin(arg);
		}

		static double cos(double arg) {
			return ::cos(arg);
		}

		static double deg2rad(double degrees) {
			return degrees * DEG2RAD;
		}

		static double rad2deg(double radians) {
			return radians * RAD2DEG;
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*MATH_*/
