/*
 * Sphere.h
 *
 *  Created on: 05/12/2010
 *      Author: victor
 */

#ifndef SPHERE_H_
#define SPHERE_H_

#include "Vector3.h"

namespace engine {
 namespace util {
   	namespace u3d {

	class AABB;

	class Sphere : public Object {
		Vector3 origin;
		float radiusSquared;

	public:
		Sphere(const Vector3 orig, float rad) : origin(orig), radiusSquared(rad * rad) {
		}

		bool intersects(const Triangle& tri) const;/* {
			return (tri.distSqrd(origin) <= radiusSquared);
		}*/
		// does this intersect an aabb?
		bool intersects(const AABB& aabb) const;
		// renders a sphere


	};

   	} // u3d
  } // util
} // engine

using namespace engine::util::u3d;

#endif /* SPHERE_H_ */
