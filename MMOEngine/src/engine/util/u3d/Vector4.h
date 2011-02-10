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
	float x, y, z, w;

public:
	Vector4() {
		x = 0, y = 0, z = 0, w = 0;
	}

	Vector4(float px, float py, float pz, float pw) {
		x = px;
		y = py;
		z = pz;
		w = pw;
	}

	Vector4(const Vector4& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}

	inline void setZero() {
		x = 0, y = 0, z = 0, w = 0;
	}

	inline float& operator[](uint32 index) {
		if (index > 3)
			throw ArrayIndexOutOfBoundsException(index);

		return *(&x + index);
	}

	inline float operator[](uint32 index) const {
		if (index > 3)
			throw ArrayIndexOutOfBoundsException(index);

		return *(&x + index);
	}

};

} // u3d
} // util
} // engine

using namespace engine::util::u3d;

#endif /* VECTOR4_H_ */
