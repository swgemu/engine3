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

	class AABBNode {
	protected:
		AABB mBox; // node's bounding box
		AABBNode* mChildren[2]; // child nodes of this node
		Vector<Triangle> mTriangles; // triangles in this node

	public:
		// constructs this aabb tree node from a triangle list and creates its children recursively
		AABBNode(Vector<Triangle>& trilist, int depth, const AABBTreeHeuristic& heurdata);
		// destructs this node
		~AABBNode();
		// renders tris that collide with the sphere

		bool testCollide(const Sphere& testsphere) const;

		//only checks boxes, triangles not implemented yet
		bool intersects(const Ray& ray, float t1, bool checkPrimitives = false);

		//returns all the triangles from the mesh
		void getTriangles(Vector<Triangle>& triangles) {
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
