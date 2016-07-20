/*
 * Vector4.h
 *
 *  Created on: 11/12/2010
 *      Author: victor
 */

#ifndef VECTOR4_H_
#define VECTOR4_H_

#include "system/lang/ArrayIndexOutOfBoundsException.h"

namespace engine {
namespace util {
namespace u3d {

class Vector4 {
protected:
	float v[4];

public:
	Vector4() {
		memset(v, 0, sizeof(v));
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
		memcpy(v, vector.v, sizeof(v));
	}

	Vector4& operator=(const Vector4& vector) {
		if (this == &vector)
			return *this;

		memcpy(v, vector.v, sizeof(v));

		return *this;
	}

	//Boolean operators
	inline bool operator==(const Vector4& vec) const {
		return memcmp(v, vec.v, sizeof(v));
	}

	inline void setZero() {
		memset(v, 0, sizeof(v));
	}

	inline bool isZero() const {
		return v[0] == 0 && v[1] == 0 && v[2] == 0 && v[3] == 0;
	}

	inline float& operator[](uint32 index) {
		if (index > 3)
			throw ArrayIndexOutOfBoundsException(index);

		return v[index];
	}

	inline float operator[](uint32 index) const {
		if (index > 3)
			throw ArrayIndexOutOfBoundsException(index);

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

#endif /* VECTOR4_H_ */
