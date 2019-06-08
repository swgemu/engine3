/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * AABB.h
 *
 *  Created on: 11/12/2010
 *      Author: victor
 */

#ifndef AABB_H_
#define AABB_H_

#include "Vector3.h"
#include "Triangle.h"
#include "Ray.h"

namespace engine {
  namespace util {
	namespace u3d {

 class AABB {
	 Vector3 mBounds[2]; // box bounds - min followed by max
 public:
	 AABB() {
	 }

	 AABB(const Vector3& boxmin, const Vector3& boxmax) {
		 mBounds[0] = boxmin;
		 mBounds[1] = boxmax;
	 }

	 // calc aabb from trilist
	 AABB(const ArrayList<Triangle*>& triangles);

	 AABB(const AABB& aab) {
		 mBounds[0] = aab.mBounds[0];
		 mBounds[1] = aab.mBounds[1];
	 }

	 bool toBinaryStream(ObjectOutputStream* stream) {
		 mBounds[0].toBinaryStream(stream);
		 mBounds[1].toBinaryStream(stream);

		 return true;
	 }

	 bool parseFromBinaryStream(ObjectInputStream* stream) {
		 mBounds[0].parseFromBinaryStream(stream);
		 mBounds[1].parseFromBinaryStream(stream);

		 return true;
	 }

	 // returns a sub box of the octant specified, with the partition as the new corner.
	 float distSqrd(const Vector3& point) const;

	 // calculate the floating point error metric
	 float errorMetric() const {
		 return (Vector3(1.0f) + (mBounds[1] - mBounds[0])).product();
	 }

	 // intersection scalar (used for weighting in building aabb)
	 uint32 longestAxis() const;
	 // mid-point

	 Vector3 midPoint() const {
		 return (mBounds[0] + mBounds[1]) * .5f;
	 }

	 inline Vector3 center() const {
		 return midPoint();
	 }

	 Vector3 extents() const {
		 return (mBounds[1] - mBounds[0]) * .5f;
	 }
	 // copy
	 void copy(const AABB& box) {
		 mBounds[0] = box.mBounds[0];
		 mBounds[1] = box.mBounds[1];
	 }

	 // make this box encompass the current box as well as this one - returns * this
	 void encompass(const AABB& encbox);


	 /*
	  * Ray-box intersection using IEEE numerical properties to ensure that the
	  * test is both robust and efficient, as described in:
	  *
	  *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
	  *      "An Efficient and Robust Ray-Box Intersection Algorithm"
	  *      Journal of graphics tools, 10(1):49-54, 2005
	  *
	  */

	 bool intersects(const Ray &r, float t0, float t1) const;

	 inline const Vector3* getMinBound() const {
		 return &mBounds[0];
	 }

	 inline Vector3* getMinBound() {
		 return &mBounds[0];
	 }

	 inline float getXMin() const {
		 return mBounds[0].getX();
	 }

	 inline float getYMin() const {
		 return mBounds[0].getY();
	 }

	 inline float getZMin() const {
		 return mBounds[0].getZ();
	 }

	 inline float getXMax() const {
		 return mBounds[1].getX();
	 }

	 inline float getYMax() const {
		 return mBounds[1].getY();
	 }

	 inline float getZMax() const {
		 return mBounds[1].getZ();
	 }

	 inline Vector3* getMaxBound() {
		 return &mBounds[1];
	 }

	 inline const Vector3* getMaxBound() const {
		 return &mBounds[1];
	 }

 };

	} // u3d
  } // util
} // engine

#endif /* AABB_H_ */
