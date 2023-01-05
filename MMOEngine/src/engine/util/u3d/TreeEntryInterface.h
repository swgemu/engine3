/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * TreeEntryInterface.h
 *
 *  Created on: 15 de ago. de 2015
 *      Author: victor
 */

#ifndef SRC_ENGINE_UTIL_U3D_TreeEntryInterface_H_
#define SRC_ENGINE_UTIL_U3D_TreeEntryInterface_H_

#include "engine/util/u3d/BasicTreeNode.h"

#include "system/util/SortedVector.h"

namespace engine {
namespace util {
namespace u3d {


template<class Node>
class TreeEntryInterfaceBase {
protected:
	Node* node;
	bool bounding;

public:
	TreeEntryInterfaceBase() : node(nullptr), bounding(false) {

	}

	TreeEntryInterfaceBase(Node* node) : node(node), bounding(false) {

	}

	virtual ~TreeEntryInterfaceBase() {

	}

	virtual bool isInRange(float x, float y, float range) const {
		float rangesq = range * range;

		float deltaX = x - getPositionX();
		float deltaY = y - getPositionY();

		if (deltaX * deltaX + deltaY * deltaY <= rangesq)
			return true;
		else
			return false;
	}

	virtual bool isInSWArea(const Node* node) const {
		float x = getPositionX();
		float y = getPositionY();

		return x >= node->minX && x < node->dividerX &&
				y >= node->minY && y < node->dividerY;
	}

	virtual bool isInSEArea(const Node* node) const {
		float x = getPositionX();
		float y = getPositionY();

		return x >= node->dividerX && x < node->maxX &&
				y >= node->minY && y < node->dividerY;
	}

	virtual bool isInNWArea(const Node* node) const {
		float x = getPositionX();
		float y = getPositionY();

		return x >= node->minX && x < node->dividerX &&
					y >= node->dividerY && y < node->maxY;
	}

	virtual bool isInArea(const Node* node) const {
		float x = getPositionX();
		float y = getPositionY();

		return (x > node->dividerX && x < node->dividerX) ||
					(y > node->dividerY && y < node->dividerY);
	}

	virtual float getPositionX() const = 0;
	virtual float getPositionY() const = 0;

	virtual int compareTo(const TreeEntryInterfaceBase* obj) const = 0;

	virtual bool isInQuadTree() const {
		return node != nullptr;
	}

	virtual Node* getNode() {
		return node;
	}

	virtual const Node* getNode() const {
		return node;
	}

	virtual uint64 getObjectID() const = 0 ;

	virtual bool isBounding() const {
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

class TreeEntryInterface : public TreeEntryInterfaceBase<BasicTreeNode> {
};

} // u3d
} // namespace util
} // namespace engine

using namespace engine::util::u3d;

#endif /* SRC_ENGINE_UTIL_U3D_TreeEntryInterface_H_ */
