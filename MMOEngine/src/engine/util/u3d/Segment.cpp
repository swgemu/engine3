/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * Segment.cpp
 *
 *  Created on: 11/03/2011
 *      Author: victor
 */

#include "Segment.h"

Segment::Segment() {

}

Segment::Segment(const Vector3& pointA, const Vector3& pointB) : pointA(pointA), pointB(pointB) {
}

Segment::Segment(const Segment& s) : pointA(s.pointA), pointB(s.pointB) {
}

Segment& Segment::operator=(const Segment& s) {
	if (this == &s)
		return *this;

	pointA = s.pointA;
	pointB = s.pointB;

	return *this;
}

Vector3 Segment::getClosestPoint(const Vector3& segmentA, const Vector3& segmentB, const Vector3& point) {
	Vector3 v = segmentB - segmentA;
	Vector3 w = point - segmentA;

	double c1 = w.dotProduct(v);

	if (c1 <= 0)
		return segmentA;

	double c2 = v.dotProduct(v);

	if (c2 <= c1)
		return segmentB;

	double b = c1 / c2;

	return segmentA + b * v;
}

Vector3 Segment::getClosestPointTo(const Vector3& point) const {
	return getClosestPoint(pointA, pointB, point);
}
