/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * LightweightQuadTree.h
 *
 *  Created on: 15 de ago. de 2015
 *      Author: victor
 */

#ifndef SRC_ENGINE_UTIL_U3D_BASICQUADTREE_H_
#define SRC_ENGINE_UTIL_U3D_BASICQUADTREE_H_

#include "engine/util/u3d/BasicTreeNode.h"

#include "TreeEntryInterface.h"

namespace engine {
  namespace util {
  	namespace u3d {

	class BasicQuadTree {
		BasicTreeNode* root;

		const int minSquareSize;

		static bool logTree;
	public:
		BasicQuadTree();
		BasicQuadTree(float minx, float miny, float maxx, float maxy, float minSquareSize = 8);

		virtual ~BasicQuadTree();

		/**
		 * Clear all the objects from the quadtree and set it to have
		 * given size.
		 */
		void setSize(float minx, float miny, float maxx, float maxy);

		/**
		 * Insert a object into the quad tree.
		 */
		virtual void insert(TreeEntryInterface *obj);

	 	/**
		 * Remove the object from the quad tree.
		 */
		virtual void remove(TreeEntryInterface *obj);

		/*
		 * Recursively iterates the tree counting the nodes
		 */
		int getNodeCount();

		/*
		 * Remove the node and all of its children including the objects
		 */
		void remove(BasicTreeNode* node);

		virtual int inRange(float x, float y, float range, Vector<TreeEntryInterface*>& objects);

		/**
		 * Remove all objects from the quad tree
		 */
		virtual void removeAll();

	private:
		void _insert(BasicTreeNode* node, TreeEntryInterface *obj);

		int _inRange(BasicTreeNode* node, float x, float y, float range, Vector<TreeEntryInterface* >& objects);

	public:
		static void setLogging(bool doLog) {
			logTree = doLog;
		}

		inline static bool doLog() {
			return logTree;
		}
	};

  	} // u3d
  } // namespace util
} // namespace engine

using namespace engine::util::u3d;



#endif /* SRC_ENGINE_UTIL_U3D_BASICQUADTREE_H_ */
