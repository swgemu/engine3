/*
 * AABBNode.h
 *
 *  Created on: 12/12/2010
 *      Author: victor
 */

#ifndef AABBNODE_H_
#define AABBNODE_H_

namespace engine {
 namespace util {
	namespace u3d {

 	struct AABBTreeHeuristic;

 	class IntersectionResult : public Variable {
 		float intersectionDistance;
 		Triangle* triangle;

 	public:
 		IntersectionResult() : Variable() {
 			intersectionDistance = 0;
 			triangle = NULL;
 		}

 		IntersectionResult(float intersectionDistance, Triangle* triangle) : Variable() {
 			this->intersectionDistance = intersectionDistance;
 			this->triangle = triangle;
 		}

 		IntersectionResult(const IntersectionResult& r)  : Variable() {
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

 		bool toBinaryStream(ObjectOutputStream* stream) {
 			return false;
 		}

 		bool parseFromBinaryStream(ObjectInputStream* stream) {
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

 		inline float getIntersectionDistance() {
 			return intersectionDistance;
 		}

 		inline Triangle* getTriangle() {
 			return triangle;
 		}


 	};

	class AABBNode {
	protected:
		AABB mBox; // node's bounding box
		AABBNode* mChildren[2]; // child nodes of this node
		Vector<Triangle*> mTriangles; // triangles in this node

	public:
		// constructs this aabb tree node from a triangle list and creates its children recursively
		AABBNode(Vector<Triangle*>& trilist, int depth, const AABBTreeHeuristic& heurdata);
		// destructs this node
		~AABBNode();
		// renders tris that collide with the sphere

		bool testCollide(const Sphere& testsphere) const;

		/**
		 * Checks for intersection against ray, stops on any intersection
		 * @return intersectionDistance, triangle which it intersects
		 */
		bool intersects(const Ray& ray, float distance, float& intersectionDistance, Triangle*& triangle, bool checkPrimitives = false);

		/**
		 * Checks for all intersections
		 */
		int intersects(const Ray& ray, float maxDistance, SortedVector<IntersectionResult>& result);

		//returns all the triangles from the mesh
		void getTriangles(Vector<Triangle*>& triangles) {
			triangles.addAll(mTriangles);

			if (mChildren[0] != NULL) {
				mChildren[0]->getTriangles(triangles);
				mChildren[1]->getTriangles(triangles);
			}
		}

		inline AABB& getBoundingBox() {
			return mBox;
		}

		inline AABBNode* getLeftChild() {
			return mChildren[0];
		}

		inline AABBNode* getRightChild() {
			return mChildren[1];
		}

	};

	} // u3d
  } // util
} // engine

using namespace engine::util::u3d;

#endif /* AABBNODE_H_ */
