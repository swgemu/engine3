/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "system/platform.h"

#ifdef PLATFORM_WIN
#define _USE_MATH_DEFINES
#include <math.h>
#else
#include <cmath>
#endif

namespace sys {
  namespace lang {

	class Math {
	public:
		static const constexpr double PI = M_PI;
		static const constexpr double DEG2RAD = M_PI / 180.0f;
		static const constexpr double RAD2DEG = 180.0f / M_PI;
		static const constexpr double E = 2.71828182845904523536;

		static inline double sqrt(double number) {
			return ::sqrt(number);
		}

		//static float rsqrt(float number);

		//static float fabs(float f);

		template<typename T>
		constexpr static T clamp(const T& min, const T& value, const T& max) {
			return (value < min) ? min : ((value > max) ? max : value);
		}

		template<class T>
		constexpr static T linearInterpolate(const T& start, const T& end, float t) {
			return ((end - start) * t) + start;
		}

		template<typename T>
		constexpr static inline T sqr(const T& val) {
			return val * val;
		}

#ifdef PLATFORM_WIN
#undef min
#undef max
#endif

		template<typename T>
		constexpr static inline T min(const T& val1, const T& val2) {
			return val1 < val2 ? val1 : val2;
		}

		template<typename T>
		constexpr static inline T max(const T& val1, const T& val2) {
			return val1 > val2 ? val1 : val2;
		}

		static float getPrecision(float num, int digits) {
			float power = pow((float)10, digits);
			return float(floor(num * power + .05f) / power);
		}

		static inline double acos(double arg) {
			return ::acos(arg);
		}

		static inline double sin(double arg) {
			return ::sin(arg);
		}

		static inline double cos(double arg) {
			return ::cos(arg);
		}

		static constexpr inline double deg2rad(double degrees) {
			return degrees * DEG2RAD;
		}

		static constexpr inline double rad2deg(double radians) {
			return radians * RAD2DEG;
		}

		static inline double ln(float value) {
			return (log10(value) / log10(E));
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;
