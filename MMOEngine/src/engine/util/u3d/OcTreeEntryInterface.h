/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * OcTreeEntryInterface.h
 */

#ifndef SRC_ENGINE_UTIL_U3D_OCTREEENTRYINTERFACE_H_
#define SRC_ENGINE_UTIL_U3D_OCTREEENTRYINTERFACE_H_

#include "engine/util/u3d/BasicOcTreeNode.h"

#include "system/util/SortedVector.h"

namespace engine {
namespace util {
namespace u3d {

template <class Node>
class OcTreeEntryInterfaceBase {
protected:
	Node* node;
	bool bounding;

public:
	OcTreeEntryInterfaceBase() : node(nullptr), bounding(false) {
	}

	OcTreeEntryInterfaceBase(Node* node) : node(node), bounding(false) {
	}

	virtual ~OcTreeEntryInterfaceBase() {
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

	virtual bool isInRange(float x, float y, float z, float range) const {
		float rangesq = range * range;

		float deltaX = x - getPositionX();
		float deltaY = y - getPositionY();
		float deltaZ = z - getPositionZ();

		if (deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ <= rangesq)
			return true;
		else
			return false;
	}

	virtual bool isInSWArea(const Node* node) const {
		float x = getPositionX();
		float y = getPositionY();
		float z = getPositionZ();

		if (node->dividerZ != -1)
			return x >= node->minX && x < node->dividerX && y >= node->minY && y < node->dividerY && z >= node->minZ && z < node->dividerZ;
		else
			return x >= node->minX && x < node->dividerX && y >= node->minY && y < node->dividerY;
	}

	virtual bool isInSEArea(const Node* node) const {
		float x = getPositionX();
		float y = getPositionY();
		float z = getPositionZ();

		if (node->dividerZ != -1)
			return x >= node->dividerX && x < node->maxX && y >= node->minY && y < node->dividerY && z >= node->minZ && z < node->dividerZ;
		else
			return x >= node->dividerX && x < node->maxX && y >= node->minY && y < node->dividerY;
	}

	virtual bool isInNWArea(const Node* node) const {
		float x = getPositionX();
		float y = getPositionY();
		float z = getPositionZ();

		if (node->dividerZ != -1)
			return x >= node->minX && x < node->dividerX && y >= node->dividerY && y < node->maxY && z >= node->minZ && z < node->dividerZ;
		else
			return x >= node->minX && x < node->dividerX && y >= node->dividerY && y < node->maxY;
	}

	virtual bool isInNEArea(const Node* node) const {
		float x = getPositionX();
		float y = getPositionY();
		float z = getPositionZ();

		if (node->dividerZ != -1)
			return x >= node->dividerX && x < node->maxX && y >= node->dividerY && y < node->maxY && z >= node->minZ && z < node->dividerZ;
		else
			return x >= node->dividerX && x < node->maxX && y >= node->dividerY && y < node->maxY;
	}

	virtual bool isInSW2Area(const Node* node) const {
		float x = getPositionX();
		float y = getPositionY();
		float z = getPositionZ();

		if (node->dividerZ != -1)
			return x >= node->minX && x < node->dividerX && y >= node->minY && y < node->dividerY && z >= node->dividerZ && z < node->maxZ;
		else
			return false;
	}

	virtual bool isInSE2Area(const Node* node) const {
		float x = getPositionX();
		float y = getPositionY();
		float z = getPositionZ();

		if (node->dividerZ != -1)
			return x >= node->dividerX && x < node->maxX && y >= node->minY && y < node->dividerY && z >= node->dividerZ && z < node->maxZ;
		else
			return false;
	}

	virtual bool isInNE2Area(const Node* node) const {
		float x = getPositionX();
		float y = getPositionY();
		float z = getPositionZ();

		if (node->dividerZ != -1)
			return x >= node->dividerX && x < node->maxX && y >= node->dividerY && y < node->maxY && z >= node->dividerZ && z < node->maxZ;
		else
			return false;
	}

	virtual bool isInNW2Area(const Node* node) const {
		float x = getPositionX();
		float y = getPositionY();
		float z = getPositionZ();

		if (node->dividerZ != -1)
			return x >= node->minX && x < node->dividerX && y >= node->dividerY && y < node->maxY && z >= node->dividerZ && z < node->maxZ;
		else
			return false;
	}

	virtual bool isInArea(const Node* node) const {
		float x = getPositionX();
		float y = getPositionY();
		float z = getPositionZ();

		if (node->dividerZ != -1) {
			return ((x > node->dividerX && x < node->dividerX) || (y > node->dividerY && y < node->dividerY)) && (z > node->dividerZ && z < node->dividerZ);
		} else {
			return (x > node->dividerX && x < node->dividerX) || (y > node->dividerY && y < node->dividerY);
		}
	}

	virtual float getPositionX() const = 0;
	virtual float getPositionY() const = 0;
	virtual float getPositionZ() const = 0;

	virtual int compareTo(const OcTreeEntryInterfaceBase* obj) const = 0;

	virtual bool isInQuadTree() const {
		return node != nullptr;
	}

	virtual bool isInOcTree() const {
		return node != nullptr;
	}

	virtual Node* getNode() {
		return node;
	}

	virtual const Node* getNode() const {
		return node;
	}

	virtual uint64 getObjectID() const = 0;

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

class OcTreeEntryInterface : public OcTreeEntryInterfaceBase<BasicOcTreeNode> {};

} // namespace u3d
} // namespace util
} // namespace engine

using namespace engine::util::u3d;

#endif /* SRC_ENGINE_UTIL_U3D_OCTREEENTRYINTERFACE_H_ */
