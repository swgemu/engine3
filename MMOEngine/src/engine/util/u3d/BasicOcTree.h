/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * LightweightOcTree.h
 */

#ifndef SRC_ENGINE_UTIL_U3D_BASICOCTREE_H_
#define SRC_ENGINE_UTIL_U3D_BASICOCTREE_H_

#include "engine/util/u3d/BasicOcTreeNode.h"

#include "OcTreeEntryInterface.h"

namespace engine {
namespace util {
namespace u3d {

class BasicOcTree {
	BasicOcTreeNode* root;

	const int minSquareSize;

	static bool logTree;

public:
	BasicOcTree();
	BasicOcTree(float minx, float miny, float minz, float maxx, float maxy, float maxz, float minSquareSize = 8);

	virtual ~BasicOcTree();

	/**
	 * Clear all the objects from the octree and set it to have
	 * given size.
	 */
	void setSize(float minx, float miny, float minz, float maxx, float maxy, float maxz);

	/**
	 * Insert a object into the quad tree.
	 */
	virtual void insert(OcTreeEntryInterface* obj);

	/**
	 * Remove the object from the quad tree.
	 */
	virtual void remove(OcTreeEntryInterface* obj);

	/*
	 * Recursively iterates the tree counting the nodes
	 */
	int getNodeCount();

	/*
	 * Remove the node and all of its children including the objects
	 */
	void remove(BasicOcTreeNode* node);

	virtual int inRange(float x, float y, float z, float range, Vector<OcTreeEntryInterface*>& objects);

	/**
	 * Remove all objects from the quad tree
	 */
	virtual void removeAll();

private:
	void _insert(BasicOcTreeNode* node, OcTreeEntryInterface* obj);

	int _inRange(BasicOcTreeNode* node, float x, float y, float z, float range, Vector<OcTreeEntryInterface*>& objects);

public:
	static void setLogging(bool doLog) {
		logTree = doLog;
	}

	inline static bool doLog() {
		return logTree;
	}
};

} // namespace u3d
} // namespace util
} // namespace engine

using namespace engine::util::u3d;

#endif /* SRC_ENGINE_UTIL_U3D_BASICOCTREE_H_ */
