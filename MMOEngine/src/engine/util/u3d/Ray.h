/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * Ray.h
 *
 *  Created on: 11/12/2010
 *      Author: victor
 */

#ifndef RAY_H_
#define RAY_H_

/*
 * Ray class, for use with the optimized ray-box intersection test
 * described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

#include "Vector3.h"

namespace engine {
namespace util {
namespace u3d {

class Ray {
protected:
	Vector3 origin;
	Vector3 direction;
	Vector3 invDirection;
	int sign[3];

public:
	Ray() {

	}

	Ray(const Vector3& o, const Vector3& d) : origin(o), direction(d), invDirection(1 / d.getX(), 1 / d.getY(), 1 / d.getZ()) {
		sign[0] = (invDirection.getX() < 0);
		sign[1] = (invDirection.getY() < 0);
		sign[2] = (invDirection.getZ() < 0);
	}

	Ray(const Ray &r) : origin(r.origin), direction(r.direction), invDirection(r.invDirection) {
		sign[0] = r.sign[0]; sign[1] = r.sign[1]; sign[2] = r.sign[2];
	}

	inline const Vector3& getOrigin() const {
		return origin;
	}

	inline const Vector3& getDirection() const {
		return direction;
	}

	inline Vector3& getOrigin() {
		return origin;
	}

	inline Vector3& getDirection() {
		return direction;
	}

	friend class AABB;
	friend class Triangle;
};

} // u3d
} // util
} // engine

using namespace engine::util::u3d;

#endif /* RAY_H_ */
