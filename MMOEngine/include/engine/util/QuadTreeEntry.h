/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef QUADTREEENTRY_H_
#define QUADTREEENTRY_H_

#include "Coordinate.h"

namespace engine {
  namespace util {

	class QuadTree;
	class QuadTreeNode;

	class QuadTreeEntry : public Coordinate {
	protected:
		QuadTreeNode* node;
		bool bounding;

		//int visibilityRange;

		SortedVector<QuadTreeEntry*> closeobjects;

		float radius;

	public:
		QuadTreeEntry(QuadTreeNode* n = NULL) : Coordinate() {
			node = n;
			bounding = false;

			//visibilityRange = 128;

			closeobjects.setInsertPlan(SortedVector<QuadTreeEntry*>::NO_DUPLICATE);

			radius = 0.5f;
		}

		virtual void notifyAddedToCloseObjects() {

		}

		virtual void notifyRemovedFromCloseObjects() {

		}

		inline void addInRangeObject(QuadTreeEntry* obj, bool doNotifyUpdate = true) {
			//System::out << "adding in range object:" << obj << "\n";

			if (closeobjects.put(obj) != -1) {
				notifyInsert(obj);
				obj->notifyAddedToCloseObjects();
			} else if (doNotifyUpdate) {
				notifyPositionUpdate(obj);
			}
		}

		inline QuadTreeEntry* getInRangeObject(int index) {
			return closeobjects.get(index);
		}

		inline void removeInRangeObject(QuadTreeEntry* obj) {
			//System::out << "droping in range object:" << obj << "\n";

			if (closeobjects.drop(obj)) {
				notifyDissapear(obj);
				obj->notifyRemovedFromCloseObjects();
			}
			/*else {
				QuadTreeEntry* obj2 = NULL;
				obj2->getPositionX();

				System::out << hex << "[0x" << obj->objectID << "] was not found in close objects for [0x"
					 << objectID << dec << "]\n";
			}*/
		}

		inline void removeInRangeObject(int index) {
			//System::out << "droping in range object with index:" << index << "\n";

			QuadTreeEntry* entry = closeobjects.remove(index);
			entry->notifyRemovedFromCloseObjects();
		}

		inline void removeInRangeObjects() {
			//System::out << "droping  all in range objects\n";

			for (int i = 0; i < closeobjects.size(); ++i) {
				closeobjects.get(i)->notifyRemovedFromCloseObjects();
			}

			closeobjects.removeAll();
		}

		inline bool containsInRangeObject(QuadTreeEntry* obj) {
			return closeobjects.contains(obj);
		}

		virtual bool isInRange(QuadTreeEntry* obj, float range) {
			return isInRange(obj->positionX, obj->positionY, range);
		}

		bool isInRange(float x, float y, float range) {
		 	float rangesq = range * range;

			float deltaX = x - positionX;
			float deltaY = y - positionY;

			if (deltaX * deltaX + deltaY * deltaY <= rangesq)
				return true;
			else
				return false;
		}

		float getDistanceTo(QuadTreeEntry* obj) {
			float x = obj->positionX;
			float y = obj->positionY;

			float deltaX = x - positionX;
			float deltaY = y - positionY;

			return sqrt(deltaX * deltaX + deltaY * deltaY);
		}

		virtual void notifyInsert(QuadTreeEntry* obj) {
		}

		virtual void notifyPositionUpdate(QuadTreeEntry* obj) {
		}

		virtual void notifyDissapear(QuadTreeEntry* obj) {
		}

		virtual int compareTo(QuadTreeEntry* obj) {
			if (getObjectID() < obj->getObjectID())
				return 1;
			else if (getObjectID() > obj->getObjectID())
				return -1;
			else
				return 0;
		}

		inline bool isInQuadTree() {
			return node != NULL;
		}

		inline int inRangeObjectCount() {
			return closeobjects.size();
		}

		virtual uint64 getObjectID() = 0;

		friend class QuadTree;
		friend class QuadTreeNode;
	};

  } // namespace util
} // namespace engine

using namespace engine::util;

#endif /*QUADTREEENTRY_H_*/
