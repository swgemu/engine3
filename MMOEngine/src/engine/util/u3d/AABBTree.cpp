/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * AABTree.cpp
 *
 *  Created on: 05/12/2010
 *      Author: victor
 */

#include "system/lang/String.h"

#include "engine/log/Logger.h"

#include "AABBTree.h"

using namespace sys::lang;

AABBTree::AABBTree(const ArrayList<Triangle*>& trilist, int depth, const AABBTreeHeuristic& heurdata, bool triangleOwner) : AABBNode(trilist, depth, heurdata), trianglesOwner(triangleOwner) {

}

AABBTree::~AABBTree() {
	if (trianglesOwner) {
		SortedVector<Triangle*> uniqueTriangles;
		uniqueTriangles.setNoDuplicateInsertPlan();

		getTriangles(uniqueTriangles);

		for (int i = 0; i < uniqueTriangles.size(); ++i) {
			delete uniqueTriangles.getUnsafe(i);
		}
	}
}

// construct an AABB from a list of triangles.
AABB::AABB(const ArrayList<Triangle*>& triangles) {
	// do nothing if no triangles in the list
	if (triangles.size() == 0)
		return;

	mBounds[0] = Vector3(0.f);
	mBounds[1] = Vector3(0.f);

	// copy aabb of first triangle
	copy(triangles.getUnsafe(0)->triAABB());

	// encompass all other triangles in the list
	for (int i = 0; i < triangles.size(); ++i) {
		encompass(triangles.getUnsafe(i)->triAABB());
	}
}

// distance squared to a point from the box (arvos algorithm)
float AABB::distSqrd(const Vector3& point) const {
	/*StringBuffer msg;
	msg << "checking against mBounds box:";
	msg << "mbounds[0] x:" << mBounds[0][0] << " y:" << mBounds[0][1] << " z:" << mBounds[0][2] << endl;
	msg << "mbounds[1] x:" << mBounds[1][0] << " y:" << mBounds[1][1] << " z:" << mBounds[1][2];

	Logger::console.info(msg.toString(), true);*/

	float dst = 0;

	//for each component, find the point's relative position and the distance contribution
	for (uint32 ii = 0; ii < 3; ++ii)
		dst += (point[ii] < mBounds[0][ii]) ? Math::sqr(point[ii] - mBounds[0][ii]) :
			 (point[ii] > mBounds[1][ii]) ? Math::sqr(point[ii] - mBounds[1][ii]) : 0.0f;

	return dst;
}

// longest axii of the bounding box
uint32 AABB::longestAxis() const {
	Vector3 boxdim(mBounds[1] - mBounds[0]);

	uint32 la = 0; // longest axis
	float lav = 0.0f; // longest axis length

	// for each dimension
	for (uint32 ii = 0; ii < 3; ++ii) {
		// check if its longer
		if (boxdim[ii] > lav) {
			// store it if it is
			la = ii;
			lav = boxdim[ii];
		}
	}

	return la;
}

// make this box encompass the current box as well as this one
void AABB::encompass(const AABB& encbox) {
	mBounds[0] = mBounds[0].getMin(encbox.mBounds[0]);
	mBounds[1] = mBounds[1].getMax(encbox.mBounds[1]);
}

bool AABB::intersects(const Ray &r, float t0, float t1) const {
	/*StringBuffer msg;
	msg << "checking ray intersect against mBounds box:";
	msg << "mbounds[0] x:" << mBounds[0][0] << " y:" << mBounds[0][1] << " z:" << mBounds[0][2] << endl;
	msg << "mbounds[1] x:" << mBounds[1][0] << " y:" << mBounds[1][1] << " z:" << mBounds[1][2];

	Logger::console.info(msg.toString(), true);*/

	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (mBounds[r.sign[0]].getX() - r.origin.getX()) * r.invDirection.getX();
	tymax = (mBounds[1 - r.sign[1]].getY() - r.origin.getY()) * r.invDirection.getY();

	if (tmin > tymax) {
		return false;
	}

	tymin = (mBounds[r.sign[1]].getY() - r.origin.getY()) * r.invDirection.getY();
	tmax = (mBounds[1 - r.sign[0]].getX() - r.origin.getX()) * r.invDirection.getX();

	if (tymin > tmax) {
		return false;
	}

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	tzmin = (mBounds[r.sign[2]].getZ() - r.origin.getZ()) * r.invDirection.getZ();
	tzmax = (mBounds[1 - r.sign[2]].getZ() - r.origin.getZ()) * r.invDirection.getZ();

	if ( (tmin > tzmax) || (tzmin > tmax) )
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	return ( (tmin < t1) && (tmax > t0) );
}

// constructs this aabb tree node from a triangle list and creates its children recursively
// note node box is initialised to the first triangle's box
AABBNode::AABBNode(const ArrayList<Triangle*>& trilist, int depth, const AABBTreeHeuristic& heurdata) : mBox(trilist) {
	// test our build heuristic - if passes, make children
	if (depth < (int)heurdata.maxdepth && trilist.size() > (int)heurdata.mintricnt &&
		(trilist.size() > (int)heurdata.tartricnt || mBox.errorMetric() > heurdata.minerror)) {

		//list<CTriangle>::const_iterator triitr; // iterator for looping through trilist
		ArrayList<Triangle*> tribuckets[2] = { ArrayList<Triangle*>(1, 1), ArrayList<Triangle*>(1, 1) }; // buckets of triangles
		uint32 longaxis = mBox.longestAxis(); // longest axis
		Vector3 geoavg(0.f); // geometric average - midpoint of ALL the triangles

		// go through all tris and calculate the average of the midpoints
		//for (triitr=trilist.begin(); triitr!=trilist.end(); triitr++)
		for (int i = 0; i < trilist.size(); ++i) {
			geoavg = geoavg + trilist.getUnsafe(i)->midPoint() * (1.0f / trilist.size());
		}

		// bucket tris based on their midpoint's side of the geo average in the longest axis
		//for (triitr=trilist.begin(); triitr!=trilist.end(); triitr++)
		for (int i = 0; i < trilist.size(); ++i) {
			tribuckets[geoavg[longaxis] > trilist.getUnsafe(i)->midPoint()[longaxis]].add(trilist.getUnsafe(i));
		}

		// create new children using the buckets
		mChildren[0] = new AABBNode(tribuckets[0], depth + 1, heurdata);
		mChildren[1] = new AABBNode(tribuckets[1], depth + 1, heurdata);

	} else { // otherwise the build heuristic failed, this is

		// set the first child to null (identifies a leaf)
		mChildren[0] = nullptr;
		mChildren[1] = nullptr;

		// copy triangle list
		if (heurdata.storePrimitives) {
			mTriangles = trilist;
		} else {
			mTriangles.removeAll(0, 1);
		}
	}
}

// destructor
AABBNode::~AABBNode() {
	// if a note a leaf node, delete the children.
	if (mChildren[0]) {
		delete mChildren[0];
		delete mChildren[1];
	}
}

int AABBNode::intersects(const Ray& ray, float maxDistance, SortedVector<IntersectionResult>& result) const {
	float intersectionDistance;

	if (mBox.intersects(ray, 0.f, maxDistance)) {
		if (mChildren[0]) {
			// recurse to children
			mChildren[0]->intersects(ray, maxDistance, result);
			mChildren[1]->intersects(ray, maxDistance, result);

		} else { // is a leaf
			for (int i = 0; i < mTriangles.size(); ++i) {
				//Vector3 intersectionPoint;

				if (mTriangles.getUnsafe(i)->intersects(ray, maxDistance, intersectionDistance)) {
					result.put(IntersectionResult(intersectionDistance, mTriangles.getUnsafe(i)));
				}
			}
		}
	}

	return result.size();
}

//bool intersects(const Ray& ray, float distance, float& intersectionDistance, Triangle*& triangle, bool checkPrimitives = false);
bool AABBNode::intersects(const Ray& ray, float distance, float& intersectionDistance, Triangle*& triangle, bool checkPrimitives) const {
	if (mBox.intersects(ray, 0.f, distance)) {
		if (mChildren[0]) {
			// recurse to children
			if (!mChildren[0]->intersects(ray, distance, intersectionDistance, triangle, checkPrimitives))
				return mChildren[1]->intersects(ray, distance, intersectionDistance, triangle, checkPrimitives);
			else
				return true;

		} else { // is a leaf
			if (checkPrimitives) {
				for (int i = 0; i < mTriangles.size(); ++i) {
					//Vector3 intersectionPoint;

					if (mTriangles.getUnsafe(i)->intersects(ray, distance, intersectionDistance)) {
						triangle = mTriangles.getUnsafe(i);
						return true;
					}
				}
			} else
				return true;
		}
	}

	return false;
}

bool AABBNode::testCollide(const Sphere& testsphere) const {
	// if no intersection with the node box, don't continue
	if (testsphere.intersects(mBox)) {

		//m_box.Render();

		// is not a leaf
		if (mChildren[0]) {
			// recurse to children
			if (!mChildren[0]->testCollide(testsphere))
				return mChildren[1]->testCollide(testsphere);
			else
				return true;

		} else { // is a leaf

			for (int i = 0; i < mTriangles.size(); ++i) {
				if (testsphere.intersects(*mTriangles.getUnsafe(i)))
					return true;
			}

		}
	}

	return false;
}
