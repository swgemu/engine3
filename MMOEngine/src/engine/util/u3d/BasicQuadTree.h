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

#include "engine/util/u3d/BasicQuadTreeNode.h"

#include "QuadTreeEntryInterface.h"

namespace engine {
  namespace util {
  	namespace u3d {

	class BasicQuadTree {
		BasicQuadTreeNode* root;

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
		virtual void insert(QuadTreeEntryInterface *obj);

	 	/**
		 * Remove the object from the quad tree.
		 */
		virtual void remove(QuadTreeEntryInterface *obj);

		/*
		 * Recursively iterates the tree counting the nodes
		 */
		int getNodeCount();

		/*
		 * Remove the node and all of its children including the objects
		 */
		void remove(BasicQuadTreeNode* node);

		virtual int inRange(float x, float y, float range, Vector<QuadTreeEntryInterface*>& objects);

		/**
		 * Remove all objects from the quad tree
		 */
		virtual void removeAll();

	private:
		void _insert(BasicQuadTreeNode* node, QuadTreeEntryInterface *obj);

		int _inRange(BasicQuadTreeNode* node, float x, float y, float range, Vector<QuadTreeEntryInterface* >& objects);

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
