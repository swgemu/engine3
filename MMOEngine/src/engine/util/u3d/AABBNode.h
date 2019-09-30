/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * AABBNode.h
 *
 *  Created on: 12/12/2010
 *      Author: victor
 */

#ifndef AABBNODE_H_
#define AABBNODE_H_

#include "AABB.h"

namespace sys {
 namespace io {
 	class ObjectOutputStream;
	class ObjectInputStream;
 }
}

namespace engine {
 namespace util {
	namespace u3d {

 	struct AABBTreeHeuristic;
	class Triangle;
	class Sphere;

 	class IntersectionResult {
 		float intersectionDistance;
 		Triangle* triangle;

 	public:
 		IntersectionResult() {
 			intersectionDistance = 0;
 			triangle = nullptr;
 		}

 		IntersectionResult(float intersectionDistance, Triangle* triangle) {
 			this->intersectionDistance = intersectionDistance;
 			this->triangle = triangle;
 		}

 		IntersectionResult(const IntersectionResult& r) {
 			intersectionDistance = r.intersectionDistance;
 			triangle = r.triangle;
 		}

 		IntersectionResult& operator=(const IntersectionResult& r) {
 			if (this == &r)
 				return *this;

 			intersectionDistance = r.intersectionDistance;
 			triangle = r.triangle;

 			return *this;
 		}

 		bool toBinaryStream(sys::io::ObjectOutputStream* stream) {
 			return false;
 		}

 		bool parseFromBinaryStream(sys::io::ObjectInputStream* stream) {
 			return false;
 		}

 		int compareTo(const IntersectionResult& r) const {
 			if (intersectionDistance < r.intersectionDistance)
 				return 1;
 			else if (intersectionDistance > r.intersectionDistance)
 				return -1;
 			else
 				return 0;
 		}

 		inline float getIntersectionDistance() const {
 			return intersectionDistance;
 		}

 		inline Triangle* getTriangle() {
 			return triangle;
 		}

		inline const Triangle* getTriangle() const {
			return triangle;
		}

 	};

	class AABBNode {
	protected:
		AABB mBox; // node's bounding box
		AABBNode* mChildren[2]; // child nodes of this node
		ArrayList<Triangle*> mTriangles; // triangles in this node

	public:
		// constructs this aabb tree node from a triangle list and creates its children recursively
		AABBNode(const ArrayList<Triangle*>& trilist, int depth, const AABBTreeHeuristic& heurdata);
		// destructs this node
		virtual ~AABBNode();
		// renders tris that collide with the sphere

		bool testCollide(const Sphere& testsphere) const;

		/**
		 * Checks for intersection against ray, stops on any intersection
		 * @return intersectionDistance, triangle which it intersects
		 */
		bool intersects(const Ray& ray, float distance, float& intersectionDistance, Triangle*& triangle, bool checkPrimitives = false) const;

		/**
		 * Checks for all intersections
		 */
		int intersects(const Ray& ray, float maxDistance, SortedVector<IntersectionResult>& result) const;

		//returns all the triangles from the mesh
		void getTriangles(ArrayList<Triangle*>& triangles) const {
			triangles.addAll(mTriangles);

			if (mChildren[0] != nullptr) {
				mChildren[0]->getTriangles(triangles);
				mChildren[1]->getTriangles(triangles);
			}
		}

		//returns all triangles from the mesh sorted
		void getTriangles(SortedVector<Triangle*>& triangles) const {
			for (int i = 0; i < mTriangles.size(); ++i) {
				triangles.put(mTriangles.getUnsafe(i));
			}

			if (mChildren[0] != nullptr) {
				mChildren[0]->getTriangles(triangles);
				mChildren[1]->getTriangles(triangles);
			}

		}

		inline AABB& getBoundingBox() {
			return mBox;
		}

		inline const AABB& getBoundingBox() const {
			return mBox;
		}

		inline AABBNode* getLeftChild() {
			return mChildren[0];
		}

		inline const AABBNode* getLeftChild() const {
			return mChildren[0];
		}

		inline AABBNode* getRightChild() {
			return mChildren[1];
		}

		inline const AABBNode* getRightChild() const {
			return mChildren[1];
		}

	};

	} // u3d
  } // util
} // engine

using namespace engine::util::u3d;

#endif /* AABBNODE_H_ */
