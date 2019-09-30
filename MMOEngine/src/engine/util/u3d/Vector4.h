/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * Vector4.h
 *
 *  Created on: 11/12/2010
 *      Author: victor
 */

#pragma once

#include "system/platform.h"
#include "system/lang/ArrayIndexOutOfBoundsException.h"

namespace engine {
namespace util {
namespace u3d {

class Vector4 {
protected:
	float v[4];

public:
	Vector4() {
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		v[3] = 0;
	}

	Vector4(float px, float py, float pz, float pw) {
		v[0] = px;
		v[1] = py;
		v[2] = pz;
		v[3] = pw;
	}

	Vector4(const float values[4]) {
		memcpy(v, values, sizeof(v));
	}

	Vector4(const Vector4& vector) {
		v[0] = vector.v[0];
		v[1] = vector.v[1];
		v[2] = vector.v[2];
		v[3] = vector.v[3];
	}

	Vector4& operator=(const Vector4& vector) {
		if (this == &vector)
			return *this;

		v[0] = vector.v[0];
		v[1] = vector.v[1];
		v[2] = vector.v[2];
		v[3] = vector.v[3];

		return *this;
	}

	//Boolean operators
	inline bool operator==(const Vector4& vec) const {
		return memcmp(v, vec.v, sizeof(v));
	}

	inline void setZero() {
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		v[3] = 0;
	}

	inline bool isZero() const {
		return v[0] == 0 && v[1] == 0 && v[2] == 0 && v[3] == 0;
	}

	inline float& operator[](uint32 index) {
#ifdef VECTORS_OUT_OF_BOUNDS_CHECK
		if (index > 3)
			throw ArrayIndexOutOfBoundsException(index);
#endif
		return v[index];
	}

	inline float operator[](uint32 index) const {
#ifdef VECTORS_OUT_OF_BOUNDS_CHECK
		if (index > 3)
			throw ArrayIndexOutOfBoundsException(index);
#endif
		return v[index];
	}

	inline float* toFloatArray() {
		return v;
	}

	inline const float* toFloatArray() const {
		return v;
	}

};

} // u3d
} // util
} // engine

using namespace engine::util::u3d;

