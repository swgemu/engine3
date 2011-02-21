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
		float vertices[9]; // optimizing for ram...

	public:
		Triangle() {

		}

		Triangle(const Triangle& tri) : Variable() {
			for (int i = 0; i < 9; ++i) {
				vertices[i] = tri.vertices[i];
			}
		}

		Triangle(const Vector3 vert[]) {
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

		Triangle& operator=(const Triangle& tri) {
			for (int i = 0; i < 9; ++i) {
				vertices[i] = tri.vertices[i];
			}

			return *this;
		}

		void transform(const Matrix4& worldMatrix) {
			for (int i = 0; i < 9; i += 3) {
				Vector3 vertex(vertices[i], vertices[i + 1], vertices[i + 2]);
				vertex = vertex * worldMatrix;

				vertices[i] = vertex.getX();
				vertices[i + 1] = vertex.getY();
				vertices[i + 2] = vertex.getZ();
			}
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			for (int i = 0; i < 9; ++i) {
				TypeInfo<float>::toBinaryStream(&vertices[i], stream);
			}

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			for (int i = 0; i < 9; ++i) {
				TypeInfo<float>::parseFromBinaryStream(&vertices[i], stream);
			}

			return true;
		}

		// distance squared to a point from the tri
		//float distSqrd(const Vector3& point) const;

		// calculate the midpoint
		Vector3 midPoint() const {
			Vector3 vert0(vertices[0], vertices[1], vertices[2]);
			Vector3 vert2(vertices[3], vertices[4], vertices[5]);
			Vector3 vert1(vertices[6], vertices[7], vertices[8]);

			return (vert0 + vert1 + vert2) * (1.0f / 3.0f);
		}

		AABB triAABB() const;

		//    Input:  a ray Ray
		//    Output: intersectionPoint = intersection point (when it exists)
		//    Return: -1 = triangle is degenerate (a segment or point)
		//             0 = disjoint (no intersect)
		//             1 = intersect in unique point I1
		//             2 = are in the same plane
		//int intersects(const Ray& ray, Vector3& intersectionPoint);

		bool intersects(const Ray& ray, float maxDistance, float& intersectionDistance);

		Vector3 getNormal() const {
			Vector3 normal;

			Vector3 vert0(vertices[0], vertices[1], vertices[2]);
			Vector3 vert2(vertices[3], vertices[4], vertices[5]);
			Vector3 vert1(vertices[6], vertices[7], vertices[8]);

			Vector3 v1 = vert1 - vert0;
			Vector3 v2 = vert2 - vert0;

			normal = v1.crossProduct(v2);

			normal.normalize();

			return normal;
		}

		inline Vector3 getVertex(int i) const {
			if (i > 2)
				ArrayIndexOutOfBoundsException(i);

			Vector3 vert(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);

			return vert;
		}
	};

  	} // u3d
  } // util
} // engine

using namespace engine::util::u3d;


#endif /* TRIANGLE_H_ */
