/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * LightweightQuadTreeNode.h
 *
 *  Created on: 15 de ago. de 2015
 *      Author: victor
 */

#ifndef SRC_ENGINE_UTIL_U3D_BASICQUADTREENODE_H_
#define SRC_ENGINE_UTIL_U3D_BASICQUADTREENODE_H_

#include "system/util/SortedVector.h"

namespace engine {
namespace util {
namespace u3d {

class QuadTreeEntryInterface;

class BasicQuadTreeNode {
protected:
	SortedVector<engine::util::u3d::QuadTreeEntryInterface*> objects;

	BasicQuadTreeNode* parentNode;
	BasicQuadTreeNode* nwNode;
	BasicQuadTreeNode* neNode;
	BasicQuadTreeNode* swNode;
	BasicQuadTreeNode* seNode;

	float minX, minY;
	float maxX, maxY;

	float dividerX, dividerY;

public:
	BasicQuadTreeNode();
	BasicQuadTreeNode(float minx, float miny, float maxx, float maxy,
			BasicQuadTreeNode *parent);

	~BasicQuadTreeNode();

	// Add a object to this node
	void addObject(QuadTreeEntryInterface *obj);

	QuadTreeEntryInterface* getObject(int index) const {
		return objects.get(index);
	}

	// Remove a object by GUID
	void removeObject(QuadTreeEntryInterface *obj);

	void removeObject(int index);

	// Approximative test if a circle with center in x,y and
	// given radius crosses this node.
	bool testInRange(float x, float y, float range) const;

	// Check if this node makes any sense to exist
	void check();

	bool validateNode() const {
		if (minX > maxX || minY > maxY)
			return false;
		else
			return true;
	}

	// Check if this node has any associated objects
	inline bool isEmpty() const {
		return objects.isEmpty();
	}

	// Check if this node has children nodes
	inline bool hasSubNodes() const {
		return nwNode != nullptr || neNode != nullptr || swNode != nullptr || seNode
				!= nullptr;
	}

	// Test if the point is inside this node
	inline bool testInside(float x, float y) const {
		return x >= minX && x < maxX && y >= minY && y < maxY;
	}

	// Test if the object is inside this node
	bool testInside(QuadTreeEntryInterface* obj) const;

	/**
	 * gets the node count including this one
	 */
	int getNodeCount() const;

	String toStringData() const;

private:
	static int _getSubNodeCount(const BasicQuadTreeNode* s);

	friend class BasicQuadTree;
	friend class QuadTreeEntryInterface;
	template<class Node>
	friend class QuadTreeEntryInterfaceBase;
};

} // u3d
} // namespace util
} // namespace engine

using namespace engine::util::u3d;


#endif /* SRC_ENGINE_UTIL_U3D_BASICQUADTREENODE_H_ */
