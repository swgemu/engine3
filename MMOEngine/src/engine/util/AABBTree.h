/*
 * AABBTree.h
 *
 *  Created on: 05/12/2010
 *      Author: victor
 */

#ifndef AABBTREE_H_
#define AABBTREE_H_

#include "Vector3.h"
#include "../../system/lang.h"
#include "../../system/util/Vector.h"

#include "Triangle.h"
#include "Sphere.h"

namespace engine {
 namespace util {

	class AABB {
		Vector3 mBounds[2]; // box bounds - min followed by max
	public:

		AABB(const Vector3& boxmin, const Vector3& boxmax) {
			mBounds[0] = boxmin;
			mBounds[1] = boxmax;
		}

		// calc aabb from trilist
		AABB(const Vector<Triangle>& triangles);

		AABB(const AABB& aab) {
			mBounds[0] = aab.mBounds[0];
			mBounds[1] = aab.mBounds[1];
		}

		// returns a sub box of the octant specified, with the partition as the new corner.
		float distSqrd(const Vector3& point) const;

		// calculate the floating point error metric
		float errorMetric() const {
			return (Vector3(1.0f) + (mBounds[1] - mBounds[0])).length();
		}

		// intersection scalar (used for weighting in building aabb)
		uint32 longestAxis() const;
			// mid-point
		Vector3 midPoint() const {
			return (mBounds[0] + mBounds[1]) * .5f;
		}
		// copy
		void copy(const AABB& box) {
			mBounds[0] = box.mBounds[0];
			mBounds[1] = box.mBounds[1];
		}

		// make this box encompass the current box as well as this one - returns * this
		void encompass(const AABB& encbox);
		// render this box
		//void Render() const;



	};

	class AABBNode {
		AABB mBox; // node's bounding box
		AABBNode* mChildren[2]; // child nodes of this node
		Vector<Triangle> mTriangles; // triangles in this node

	public:
		// heuristic struct, passed down the tree
		struct Heuristic {
			uint8 maxdepth; // maximum depth
			uint32 mintricnt; // minimum triangle count
			uint32 tartricnt; // target triangle count
			float minerror; // minimum error required
		};

		// constructs this aabb tree node from a triangle list and creates its children recursively
		AABBNode(Vector<Triangle>& trilist, int depth, const Heuristic& heurdata);
		// destructs this node
		~AABBNode();
		// renders tris that collide with the sphere

		bool testCollide(const Sphere& testsphere) const;

	};

 }
}

#endif /* AABTREE_H_ */
