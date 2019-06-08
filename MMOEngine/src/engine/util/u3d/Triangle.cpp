/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * Triangle.cpp
 *
 *  Created on: 11/03/2011
 *      Author: victor
 */

#include "Triangle.h"
#include "AABBTree.h"

#define LOCAL_EPSILON FLT_EPSILON

#ifdef TRIANGLE_INHERITS_VARIABLE
Triangle::Triangle(const Triangle& tri) noexcept : Variable() {
	memcpy(verts, tri.verts, sizeof(verts));
}

Triangle& Triangle::operator=(const Triangle& tri) noexcept {
	if (this == &tri)
		return *this;

	memcpy(verts, tri.verts, sizeof(verts));

	return *this;
}
#endif

Triangle::Triangle(const Vector3 vert[3]) noexcept {
	set(vert);
}

int Triangle::compareTo(const Triangle* object) const {
	if (this == object)
		return 0;
	else if (this < object)
		return 1;
	else
		return -1;
}

void Triangle::transform(const Matrix4& worldMatrix) {
	for (int i = 0; i < 3; i++) {
		Vector3& vertex = verts[i]; //(vertices[i], vertices[i + 1], vertices[i + 2]);

		vertex = worldMatrix * vertex;
	}
}

bool Triangle::toBinaryStream(ObjectOutputStream* stream) {
	for (int i = 0; i < 3; ++i) {
		TypeInfo<Vector3>::toBinaryStream(&verts[i], stream);
	}

	return true;
}

bool Triangle::parseFromBinaryStream(ObjectInputStream* stream) {
	for (int i = 0; i < 3; ++i) {
		TypeInfo<Vector3>::parseFromBinaryStream(&verts[i], stream);
	}

	return true;
}

// distance squared to a point from the tri
//float distSqrd(const Vector3& point) const;

// calculate the midpoint
Vector3 Triangle::midPoint() const {
	const Vector3& vert0 = verts[0];
	const Vector3& vert2 = verts[1];
	const Vector3& vert1 = verts[2];

	return (vert0 + vert1 + vert2) * (1.0f / 3.0f);
}

Vector3 Triangle::getNormal() const {
	const Vector3& vert0 = verts[0];
	const Vector3& vert2 = verts[1];
	const Vector3& vert1 = verts[2];

	Vector3 v1 = vert1 - vert0;
	Vector3 v2 = vert2 - vert0;

	Vector3 normal = v1.crossProduct(v2);

	normal.normalize();

	return normal;
}

float Triangle::area() const {
	const Vector3& v0 = verts[0];
	const Vector3& v2 = verts[1];
	const Vector3& v1 = verts[2];

	return (float) 0.5 * ((v1 - v0).crossProduct(v2 - v0)).length();
}

float Triangle::area2D() const {
	float ax = verts[1].getX() - verts[0].getX();
	float ay = verts[1].getZ() - verts[0].getZ();

	float bx = verts[2].getX() - verts[0].getX();
	float by = verts[2].getZ() - verts[0].getZ();

	return bx * ay - ax * by;
}

// get the aabb for this triangle (used for making group aabb)
AABB Triangle::triAABB() const {
	const Vector3& vert0 = verts[0];
	const Vector3& vert2 = verts[1];
	const Vector3& vert1 = verts[2];

	// new aabb with min as minimum of 3 verts and maximum max of 3 verts
	return AABB(vert0.getMin(vert1).getMin(vert2),
			vert0.getMax(vert1).getMax(vert2));
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

bool Triangle::intersects(const Ray& ray, float maxDistance, float& intersectionDistance) const {
	intersectionDistance = 0;

	/*
	Vector3    u, v, n;             // triangle vectors
	Vector3    dir, w0, w;          // ray vectors
	float     r, a, b;             // params to calc ray-plane intersect

	Vector3 vert0(vertices[0], vertices[1], vertices[2]);
	Vector3 vert2(vertices[3], vertices[4], vertices[5]);
	Vector3 vert1(vertices[6], vertices[7], vertices[8]);

	// get triangle edge vectors and plane normal
	u = vert1 - vert0;
	v = vert2 - vert0;
	n = u.crossProduct(v);             // cross product

	if (n == Vector3(0, 0, 0))            // triangle is degenerate
		return false;                 // do not deal with this case

	dir = ray.getDirection();             // ray direction vector
	w0 = ray.getOrigin() - vert0;

	//a = -dot(n,w0);
	a = -n.dotProduct(w0);

	//b = dot(n,dir);
	b = n.dotProduct(dir);

	if (fabs(b) < SMALL_NUM) {     // ray is parallel to triangle plane
		if (a == 0)                // ray lies in triangle plane
			return false;
		else return false;             // ray disjoint from plane
	}

	// get intersect point of ray with triangle plane
	r = a / b;
	if (r < 0.0)                   // ray goes away from triangle
		return false;                  // => no intersect
	// for a segment, also test if (r > 1.0) => no intersect

	Vector3 intersectionPoint = ray.getOrigin() + r * dir;           // intersect point of ray and plane

	// is I inside T?
	float    uu, uv, vv, wu, wv, D;
	uu = u.dotProduct(u);
	uv = u.dotProduct(v);
	vv = v.dotProduct(v);
	w = intersectionPoint - vert0;
	wu = w.dotProduct(u);
	wv = w.dotProduct(v);
	D = uv * uv - uu * vv;

	// get and test parametric coords
	float s, t;
	s = (uv * wv - vv * wu) / D;
	if (s < 0.0 || s > 1.0)        // I is outside T
		return false;
	t = (uv * wu - uu * wv) / D;
	if (t < 0.0 || (s + t) > 1.0)  // I is outside T
		return false;

	intersectionDistance = r;

	return true;                      // I is in T


*/


// Find vectors for two edges sharing vert0
	const Vector3& vert0 = verts[0]; //(vertices[0], vertices[1], vertices[2]);
	const Vector3& vert2 = verts[1]; //(vertices[3], vertices[4], vertices[5]);
	const Vector3& vert1 = verts[2]; //(vertices[6], vertices[7], vertices[8]);

	Vector3 edge1 = vert1 - vert0;
	Vector3 edge2 = vert2 - vert0;

	// Begin calculating determinant - also used to calculate U parameter
	const Vector3& rayDirectionNormalized = ray.direction;
	//rayDirectionNormalized.normalize();
	Vector3 pvec = rayDirectionNormalized.crossProduct(edge2);

	// If determinant is near zero, ray lies in plane of triangle
	float det = edge1.dotProduct(pvec);
	// the non-culling branch

	if (det > -LOCAL_EPSILON && det < LOCAL_EPSILON)
		return false;

	float OneOverDet = 1.0f / det;

	// Calculate distance from vert0 to ray origin
	Vector3 tvec = ray.origin - vert0;

	// Calculate U parameter and test bounds
	float mU, mV;
	mU = (tvec.dotProduct(pvec)) * OneOverDet;
	//		if(IR(u)&0x80000000 || u>1.0f)					return FALSE;
	if (mU < 0.0f || mU > 1.f)
		return false;

	// prepare to test V parameter
	Vector3 qvec = tvec.crossProduct(edge1);

	// Calculate V parameter and test bounds
	mV = (rayDirectionNormalized.dotProduct(qvec)) * OneOverDet;
	if (mV < 0.f || mU + mV > 1.0f)
		return false;

	// Calculate t, ray intersects triangle
	float mDistance = (edge2.dotProduct(qvec)) * OneOverDet;
	// Intersection point is valid if distance is positive (else it can just be a face behind the orig point)
	if (mDistance < 0)
		return false;

	//String a = "ray/triangle intersecting with mDistance:";
	//a += String::valueOf(mDistance);
	//a += " maxDistance:";
	//a += String::valueOf(maxDistance);
	//Logger::console.info(a, true);

	if (mDistance <= maxDistance) {
		intersectionDistance = mDistance;

		return true;
	} else
		return false;

	//return true;

}

int Triangle::getSharedVertices(const Triangle& tri, const Vector3*& vertexA, const Vector3*& vertexB) const {
	int count = 0;

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (getVertex(i) == tri.getVertex(j)) {
				if (++count == 2) {
					vertexB = &getVertex(i);

					return 0;
				} else
					vertexA = &getVertex(i);
			}
		}
	}

	return 1;
}

const Vector3& Triangle::getLeftSharedVertex(const Triangle& tri) const {
	int firstCommon = -1, secondCommon = -1;
	int apexID = -1;

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (getVertex(i) == tri.getVertex(j)) {
				if (firstCommon == -1) {
					firstCommon = i;
				} else if (secondCommon == -1) {
					secondCommon = i;

					break;
				}
			}
		}
	}

	if (firstCommon == -1 || secondCommon == -1)
		throw Exception("Couldnt find shared vertex!");

	if (secondCommon == 1)
		apexID = 2;
	else {
		if (firstCommon == 1)
			apexID = 0;
		else
			apexID = 1;
	}

	/*
	firstCommon = Math::clamp(0, firstCommon, 3);
	secondCommon = Math::clamp(0, secondCommon, 3);
	apexID = Math::clamp(0, apexID, 3);
	*/

	const Vector3& pointA = getVertex(firstCommon);
	const Vector3& pointB = getVertex(secondCommon);
	const Vector3& apex = getVertex(apexID);

	if (area2D(apex, pointA, pointB) <= 0)
		return pointB;
	else
		return pointA;
}

const Vector3& Triangle::getRightSharedVertex(const Triangle& tri) const {
	int firstCommon = -1, secondCommon = -1;
	int apexID = -1;

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (getVertex(i) == tri.getVertex(j)) {
				if (firstCommon == -1) {
					firstCommon = i;
				} else if (secondCommon == -1) {
					secondCommon = i;

					break;
				}
			}
		}
	}

	if (firstCommon == -1 || secondCommon == -1)
		throw Exception("Couldnt find shared vertex!");

	if (secondCommon == 1)
		apexID = 2;
	else {
		if (firstCommon == 1)
			apexID = 0;
		else
			apexID = 1;
	}

	const Vector3& pointA = getVertex(firstCommon);
	const Vector3& pointB = getVertex(secondCommon);
	const Vector3& apex = getVertex(apexID);

	if (area2D(apex, pointA, pointB) <= 0)
		return pointA;
	else
		return pointB;
}

Vector3 Triangle::getBarycenter() const {
	float x = (verts[0].getX() + verts[1].getX() + verts[2].getX()) / 3.f;
	float y = (verts[0].getY() + verts[1].getY() + verts[2].getY())  / 3.f;
	float z = (verts[0].getZ() + verts[1].getZ() + verts[2].getZ())  / 3.f;

	return Vector3(x, y, z);
}
