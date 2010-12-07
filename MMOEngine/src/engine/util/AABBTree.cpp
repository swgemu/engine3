/*
 * AABTree.cpp
 *
 *  Created on: 05/12/2010
 *      Author: victor
 */

#include "AABBTree.h"
#include "../log/Logger.h"
#include "../../system/lang/String.h"

#define SQR(x) ((x) * (x))

#define MAX_FLOAT 3.4028235E38;

using namespace sys::lang;

// construct an AABB from a list of triangles.
AABB::AABB(const Vector<Triangle>& triangles) {
	// do nothing if no triangles in the list
	if (triangles.size() == 0)
		return;

	mBounds[0] = Vector3(0.f);
	mBounds[1] = Vector3(0.f);

	// copy aabb of first triangle
	copy(triangles.get(0).triAABB());

	// encompass all other triangles in the list
	for (int i = 0; i < triangles.size(); ++i) {
		encompass((&triangles.get(i))->triAABB());
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
		dst += (point[ii] < mBounds[0][ii]) ? SQR(point[ii] - mBounds[0][ii]) :
			 (point[ii] > mBounds[1][ii]) ? SQR(point[ii] - mBounds[1][ii]) : 0.0f;

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

// get the aabb for this triangle (used for making group aabb)
AABB Triangle::triAABB() const {
	// new aabb with min as minimum of 3 verts and maximum max of 3 verts
	return AABB(vertices[0].getMin(vertices[1]).getMin(vertices[2]),
			vertices[0].getMax(vertices[1]).getMax(vertices[2]));
}

// distance squared to a point from the tri
// note SMOD is just a specialised case of a modulation
/*float Triangle::distSqrd(const Vector3& point) const {
	uint32 ii;
	uint32 edgesidecount = 0; // edge side total
	uint32 edgeside[3]; // edge side (inside/outside)
	Vector3 hpnorm; // half plane normal
	Vector3 edgevec[3]; // edge vectors

	// for all edges
	for (ii = 0; ii < 3; ++ii) {
		// create an edge vector
		edgevec[ii] = vertices[ii] - vertices[ (ii + 1) % 3];

		// make a normal for the plane through the edge perpindicular to the triangle
		hpnorm = getNormal().crossProduct(edgevec[ii]);

		// test the side on which our point lies relative to this plane
		edgeside[ii] = hpnorm.dotProduct(point) <= (hpnorm.dotProduct(vertices[ii]));
		edgesidecount += edgeside[ii];
	}

	// this is closest to a point
    if (edgesidecount == 1) {
		for (ii = 0; ii < 3; ++ii) {

			// find the edge this is inside and the distance to the corresponding vert
			if (edgeside[ii])
				return (point - vertices[(ii + 2) % 3]).squaredLength();
		}
	}

	// this is closest to an edge (line segment distance)
	if (edgesidecount == 2) {
		for (ii = 0; ii < 3; ++ii) {// find the edge this is not inside
			if (!edgeside[ii]) { // then return the distance to the edge

				// point is closest the first end of the line segment
				if (edgevec[ii].dotProduct(point) > edgevec[ii].dotProduct(vertices[ii]))
					return (point - vertices[ii]).squaredLength();

				// point is closest the far end of the line segment
				if (edgevec[ii].dotProduct(point) < edgevec[ii].dotProduct(vertices[(ii + 1) % 3]))
					return (point - vertices[(ii + 1) % 3]).squaredLength();

				// point is clocest the line part of the segment
				return edgevec[ii].crossProduct(vertices[ii] - point).squaredLength() / edgevec[ii].squaredLength();
			}
		}
	}

	float test = SQR(getNormal().dotProduct(point) *//*- m_tri_d*//*);

	Logger::console.info("falling back to distance.. returning " + String::valueOf(test), true);
	// falls back to distance to the plane, so return it
	return test;
}*/

bool Sphere::intersects(const Triangle& tri) const {
	// Early exit if one of the vertices is inside the sphere
	/*StringBuffer msg;
	msg << "checking against triangle:";
	msg << "vertex[0] x:" << tri.getVertex(0)[0] << " y:" << tri.getVertex(0)[1] << " z:" << tri.getVertex(0)[2] << endl;
	msg << "vertex[1] x:" << tri.getVertex(1)[0]<< " y:" << tri.getVertex(1)[1] << " z:" << tri.getVertex(1)[2];
	msg << "vertex[2] x:" << tri.getVertex(2)[0]<< " y:" << tri.getVertex(2)[1] << " z:" << tri.getVertex(2)[2];

	Logger::console.info(msg.toString(), true);*/

	float mRadius2 = radiusSquared;

	Vector3 mCenter = origin;
	Vector3 vert0 = tri.getVertex(0);
	Vector3 vert1 = tri.getVertex(1);
	Vector3 vert2 = tri.getVertex(2);

	Vector3 kDiff = vert2 - mCenter;

	float fC = kDiff.squaredLength();
	if (fC <= mRadius2)
		return true;

	kDiff = vert1 - mCenter;
	fC = kDiff.squaredLength();
	if (fC <= mRadius2)
		return true;

	kDiff = vert0 - mCenter;
	fC = kDiff.squaredLength();
	if(fC <= mRadius2)
		return true;

	// Else do the full distance test
	Vector3 TriEdge0 = vert1 - vert0;
	Vector3 TriEdge1 = vert2 - vert0;

	//Point kDiff	= vert0 - mCenter;
	float fA00	= TriEdge0.squaredLength();
	float fA01	= TriEdge0.dotProduct(TriEdge1);
	float fA11	= TriEdge1.squaredLength();
	float fB0	= kDiff.dotProduct(TriEdge0);
	float fB1	= kDiff.dotProduct(TriEdge1);
	//float fC	= kDiff.SquareMagnitude();
	float fDet	= fabsf(fA00 * fA11 - fA01* fA01);
	float u		= fA01 * fB1 - fA11 * fB0;
	float v		= fA01 * fB0 - fA00 * fB1;
	float SqrDist;

	if(u + v <= fDet)
	{
		if(u < 0.0f)
		{
			if(v < 0.0f)  // region 4
			{
				if(fB0 < 0.0f)
				{
					//					v = 0.0f;
					if(-fB0>=fA00)			{ /*u = 1.0f;*/		SqrDist = fA00+2.0f*fB0+fC;	}
					else					{ u = -fB0/fA00;	SqrDist = fB0*u+fC;			}
				}
				else
				{
					//					u = 0.0f;
					if(fB1>=0.0f)			{ /*v = 0.0f;*/		SqrDist = fC;				}
					else if(-fB1>=fA11)		{ /*v = 1.0f;*/		SqrDist = fA11+2.0f*fB1+fC;	}
					else					{ v = -fB1/fA11;	SqrDist = fB1*v+fC;			}
				}
			}
			else  // region 3
			{
				//				u = 0.0f;
				if(fB1>=0.0f)				{ /*v = 0.0f;*/		SqrDist = fC;				}
				else if(-fB1>=fA11)			{ /*v = 1.0f;*/		SqrDist = fA11+2.0f*fB1+fC;	}
				else						{ v = -fB1/fA11;	SqrDist = fB1*v+fC;			}
			}
		}
		else if(v < 0.0f)  // region 5
		{
			//			v = 0.0f;
			if(fB0>=0.0f)					{ /*u = 0.0f;*/		SqrDist = fC;				}
			else if(-fB0>=fA00)				{ /*u = 1.0f;*/		SqrDist = fA00+2.0f*fB0+fC;	}
			else							{ u = -fB0/fA00;	SqrDist = fB0*u+fC;			}
		}
		else  // region 0
		{
			// minimum at interior point
			if(fDet==0.0f)
			{
				//				u = 0.0f;
				//				v = 0.0f;
				SqrDist = MAX_FLOAT;
			}
			else
			{
				float fInvDet = 1.0f/fDet;
				u *= fInvDet;
				v *= fInvDet;
				SqrDist = u*(fA00*u+fA01*v+2.0f*fB0) + v*(fA01*u+fA11*v+2.0f*fB1)+fC;
			}
		}
	}
	else
	{
		float fTmp0, fTmp1, fNumer, fDenom;

		if(u < 0.0f)  // region 2
		{
			fTmp0 = fA01 + fB0;
			fTmp1 = fA11 + fB1;
			if(fTmp1 > fTmp0)
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00-2.0f*fA01+fA11;
				if(fNumer >= fDenom)
				{
					//					u = 1.0f;
					//					v = 0.0f;
					SqrDist = fA00+2.0f*fB0+fC;
				}
				else
				{
					u = fNumer/fDenom;
					v = 1.0f - u;
					SqrDist = u*(fA00*u+fA01*v+2.0f*fB0) + v*(fA01*u+fA11*v+2.0f*fB1)+fC;
				}
			}
			else
			{
				//				u = 0.0f;
				if(fTmp1 <= 0.0f)		{ /*v = 1.0f;*/		SqrDist = fA11+2.0f*fB1+fC;	}
				else if(fB1 >= 0.0f)	{ /*v = 0.0f;*/		SqrDist = fC;				}
				else					{ v = -fB1/fA11;	SqrDist = fB1*v+fC;			}
			}
		}
		else if(v < 0.0f)  // region 6
		{
			fTmp0 = fA01 + fB1;
			fTmp1 = fA00 + fB0;
			if(fTmp1 > fTmp0)
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00-2.0f*fA01+fA11;
				if(fNumer >= fDenom)
				{
					//					v = 1.0f;
					//					u = 0.0f;
					SqrDist = fA11+2.0f*fB1+fC;
				}
				else
				{
					v = fNumer/fDenom;
					u = 1.0f - v;
					SqrDist = u*(fA00*u+fA01*v+2.0f*fB0) + v*(fA01*u+fA11*v+2.0f*fB1)+fC;
				}
			}
			else
			{
				//				v = 0.0f;
				if(fTmp1 <= 0.0f)		{ /*u = 1.0f;*/		SqrDist = fA00+2.0f*fB0+fC;	}
				else if(fB0 >= 0.0f)	{ /*u = 0.0f;*/		SqrDist = fC;				}
				else					{ u = -fB0/fA00;	SqrDist = fB0*u+fC;			}
			}
		}
		else  // region 1
		{
			fNumer = fA11 + fB1 - fA01 - fB0;
			if(fNumer <= 0.0f)
			{
				//				u = 0.0f;
				//				v = 1.0f;
				SqrDist = fA11+2.0f*fB1+fC;
			}
			else
			{
				fDenom = fA00-2.0f*fA01+fA11;
				if(fNumer >= fDenom)
				{
					//					u = 1.0f;
					//					v = 0.0f;
					SqrDist = fA00+2.0f*fB0+fC;
				}
				else
				{
					u = fNumer/fDenom;
					v = 1.0f - u;
					SqrDist = u*(fA00*u+fA01*v+2.0f*fB0) + v*(fA01*u+fA11*v+2.0f*fB1)+fC;
				}
			}
		}
	}

	//Logger::console.info("Squared distance from triangle " + String::valueOf(fabsf(SqrDist)), true);

	return fabsf(SqrDist) < mRadius2;
}


// constructs this aabb tree node from a triangle list and creates its children recursively
// note node box is initialised to the first triangle's box
AABBNode::AABBNode(Vector<Triangle>& trilist, int depth, const Heuristic& heurdata) : mBox(trilist) {
	// test our build heuristic - if passes, make children
	if (depth < (int)heurdata.maxdepth && trilist.size() > (int)heurdata.mintricnt &&
		(trilist.size() > (int)heurdata.tartricnt || mBox.errorMetric() > heurdata.minerror)) {

		//list<CTriangle>::const_iterator triitr; // iterator for looping through trilist
		Vector<Triangle> tribuckets[2]; // buckets of triangles
		uint32 longaxis = mBox.longestAxis(); // longest axis
		Vector3 geoavg(0.f); // geometric average - midpoint of ALL the triangles

		// go through all tris and calculate the average of the midpoints
		//for (triitr=trilist.begin(); triitr!=trilist.end(); triitr++)
		for (int i = 0; i < trilist.size(); ++i) {
			geoavg = geoavg + trilist.get(i).midPoint() * (1.0f / trilist.size());
		}

		// bucket tris based on their midpoint's side of the geo average in the longest axis
		//for (triitr=trilist.begin(); triitr!=trilist.end(); triitr++)
		for (int i = 0; i < trilist.size(); ++i) {
			tribuckets[geoavg[longaxis] > trilist.get(i).midPoint()[longaxis]].add(trilist.get(i));
		}

		// create new children using the buckets
		mChildren[0] = new AABBNode(tribuckets[0], depth + 1, heurdata);
		mChildren[1] = new AABBNode(tribuckets[1], depth + 1, heurdata);

	} else { // otherwise the build heuristic failed, this is

		// set the first child to null (identifies a leaf)
		mChildren[0] = NULL;

		// copy triangle list
		mTriangles = trilist;
	}
}

bool Sphere::intersects(const AABB& aabb) const {
	float distSqrtToOrigin = aabb.distSqrd(origin);
	bool ret = ( distSqrtToOrigin <= radiusSquared);

	StringBuffer msg;

	/*if (ret)
		msg << "intersecting with mbox distance ";
	else
		msg << "not intersecting with mbox distance";

	msg << "distSqrtToOrigin:" << distSqrtToOrigin << " distNotSquared:" << sqrt(distSqrtToOrigin);
	msg << " radiusSquared:" << radiusSquared;

	Logger::console.info(msg.toString(), true);*/

	return ret;
}

// destructor
AABBNode::~AABBNode() {
	// if a note a leaf node, delete the children.
	if (mChildren[0]) {
		delete mChildren[0];
		delete mChildren[1];
	}
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
				if (testsphere.intersects(mTriangles.get(i)))
					return true;
			}

		}
	}

	return false;
}
