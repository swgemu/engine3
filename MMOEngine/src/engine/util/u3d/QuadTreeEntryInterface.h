/*
 * QuadTreeEntryInterface.h
 *
 *  Created on: 15 de ago. de 2015
 *      Author: victor
 */

#ifndef SRC_ENGINE_UTIL_U3D_QUADTREEENTRYINTERFACE_H_
#define SRC_ENGINE_UTIL_U3D_QUADTREEENTRYINTERFACE_H_

#include "engine/util/u3d/BasicQuadTreeNode.h"

#include "system/util/SortedVector.h"

namespace engine {
namespace util {
namespace u3d {


template<class Node>
class QuadTreeEntryInterfaceBase {
protected:
	Node* node;
	bool bounding;

public:
	QuadTreeEntryInterfaceBase() : node(NULL), bounding(false) {

	}

	QuadTreeEntryInterfaceBase(Node* node) : node(node), bounding(false) {

	}

	virtual ~QuadTreeEntryInterfaceBase() {

	}

	virtual bool isInRange(float x, float y, float range) {
		float rangesq = range * range;

		float deltaX = x - getPositionX();
		float deltaY = y - getPositionY();

		if (deltaX * deltaX + deltaY * deltaY <= rangesq)
			return true;
		else
			return false;
	}

	virtual bool isInSWArea(Node* node) {
		float x = getPositionX();
		float y = getPositionY();

		return x >= node->minX && x < node->dividerX &&
				y >= node->minY && y < node->dividerY;
	}

	virtual bool isInSEArea(Node* node) {
		float x = getPositionX();
		float y = getPositionY();

		return x >= node->dividerX && x < node->maxX &&
				y >= node->minY && y < node->dividerY;
	}

	virtual bool isInNWArea(Node* node) {
		float x = getPositionX();
		float y = getPositionY();

		return x >= node->minX && x < node->dividerX &&
					y >= node->dividerY && y < node->maxY;
	}

	virtual bool isInArea(Node* node) {
		float x = getPositionX();
		float y = getPositionY();

		return (x > node->dividerX && x < node->dividerX) ||
					(y > node->dividerY && y < node->dividerY);
	}

	virtual float getPositionX() = 0;
	virtual float getPositionY() = 0;

	virtual int compareTo(QuadTreeEntryInterfaceBase* obj) = 0;

	virtual bool isInQuadTree() {
		return node != NULL;
	}

	virtual Node* getNode() {
		return node;
	}

	virtual uint64 getObjectID() = 0;

	virtual bool isBounding() {
		return bounding;
	}

	virtual void setNode(Node* n) {
		node = n;
	}

	virtual void setBounding() {
		bounding = true;
	}

	virtual void clearBounding() {
		bounding = false;
	}
};

class QuadTreeEntryInterface : public QuadTreeEntryInterfaceBase<BasicQuadTreeNode> {
};

} // u3d
} // namespace util
} // namespace engine

using namespace engine::util::u3d;

#endif /* SRC_ENGINE_UTIL_U3D_QUADTREEENTRYINTERFACE_H_ */
