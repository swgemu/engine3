/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * Sphere.h
 *
 *  Created on: 05/12/2010
 *      Author: victor
 */

#pragma once

#include "Vector3.h"
#include "Triangle.h"

namespace engine {
 namespace util {
   	namespace u3d {

	class AABB;

	class Sphere : public Object {
		Vector3 origin;
		float radius;
		float radiusSquared;

	public:
		Sphere() : radius(0), radiusSquared(0) {
		}

		Sphere(const Vector3& orig, float rad) : origin(orig), radius(rad),
			radiusSquared(rad * rad) {
		}

		bool containsPoint(const Vector3 point) const {
			return (point - getCenter()).length() < getRadius();
		}

		bool intersects(const Triangle& tri) const;/* {
			return (tri.distSqrd(origin) <= radiusSquared);
		}*/
		// does this intersect an aabb?
		bool intersects(const AABB& aabb) const;
		// renders a sphere

		inline float getRadius() const {
			return radius;
		}

		inline float getRadiusSquared() const {
			return radiusSquared;
		}

		inline Vector3& getCenter() {
			return origin;
		}

		inline const Vector3& getCenter() const {
			return origin;
		}
	};

   	} // u3d
  } // util
} // engine

using namespace engine::util::u3d;

