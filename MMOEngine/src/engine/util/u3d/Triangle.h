/*
 * Triangle.h
 *
 *  Created on: 05/12/2010
 *      Author: victor
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "Vector3.h"
#include "Matrix4.h"

namespace engine {
 namespace util {
  	namespace u3d {

	class AABB;
	class Ray;

	class Triangle : public Variable {
		//Vector3 vertices[3];
	protected:
		float vertices[9]; // optimizing for ram...

	public:
		Triangle();

		Triangle(const Triangle& tri);

		Triangle(const Vector3 vert[]);

		Triangle& operator=(const Triangle& tri);

		void transform(const Matrix4& worldMatrix);

		bool toBinaryStream(ObjectOutputStream* stream);

		bool parseFromBinaryStream(ObjectInputStream* stream);

		virtual int compareTo(const Triangle* triangle) const;

		// calculate the midpoint
		Vector3 midPoint() const;

		AABB triAABB() const;

		//    Input:  a ray Ray
		//    Output: intersectionPoint = intersection point (when it exists)
		//    Return: -1 = triangle is degenerate (a segment or point)
		//             0 = disjoint (no intersect)
		//             1 = intersect in unique point I1
		//             2 = are in the same plane
		//int intersects(const Ray& ray, Vector3& intersectionPoint);

		bool intersects(const Ray& ray, float maxDistance, float& intersectionDistance) const;

		Vector3 getNormal() const;

		/**
		 * returns 0 on success
		 */
		int getSharedVertices(Triangle* tri, Vector3& vertexA, Vector3& vertexB) const;

		Vector3 getLeftSharedVertex(Triangle* tri) const;
		Vector3 getRightSharedVertex(Triangle* tri) const;

		Vector3 getBarycenter() const;

		float area() const;

		//uses vertices x and z
		float area2D() const;

		static float area2D(const Vector3& a, const Vector3& b, const Vector3& c) {
			float ax = b[0] - a[0];
			float ay = b[2] - a[2];

			float bx = c[0] - a[0];
			float by = c[2] - a[2];

			return bx * ay - ax * by;
		}

		inline void set(const Vector3 vert[]) {
			vertices[0] = vert[0].getX();
			vertices[1] = vert[0].getY();
			vertices[2] = vert[0].getZ();

			vertices[3] = vert[1].getX();
			vertices[4] = vert[1].getY();
			vertices[5] = vert[1].getZ();

			vertices[6] = vert[2].getX();
			vertices[7] = vert[2].getY();
			vertices[8] = vert[2].getZ();
		}

		inline Vector3 getVertex(uint32 i) const {
			if (i > 2)
				throw ArrayIndexOutOfBoundsException(i);

			Vector3 vert(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);

			return vert;
		}
	};

  	} // u3d
  } // util
} // engine

using namespace engine::util::u3d;


#endif /* TRIANGLE_H_ */
