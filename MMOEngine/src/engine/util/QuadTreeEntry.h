/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef QUADTREEENTRY_H_
#define QUADTREEENTRY_H_

#include "engine/orb/object/DistributedObject.h"

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
	
		uint64 objectID;
		
		float radius;
	
	public:
		QuadTreeEntry(QuadTreeNode* n = NULL) : Coordinate() {
			node = n;
			bounding = false;
			
			//visibilityRange = 128;
			
			closeobjects.setInsertPlan(SortedVector<QuadTreeEntry*>::NO_DUPLICATE);
			
			radius = 0.5f;
		}
	
		inline void addInRangeObject(QuadTreeEntry* obj, bool doNotifyUpdate = true) {
			if (closeobjects.put(obj) != -1)
				notifyInsert(obj);
			else if (doNotifyUpdate)
				notifyPositionUpdate(obj);
		}
	
		inline QuadTreeEntry* getInRangeObject(int index) {
			return closeobjects.get(index);
		}
	
		inline void removeInRangeObject(QuadTreeEntry* obj) {
			if (closeobjects.drop(obj))
				notifyDissapear(obj);
			/*else {
				QuadTreeEntry* obj2 = NULL;
				obj2->getPositionX();

				cout << hex << "[0x" << obj->objectID << "] was not found in close objects for [0x" 
					 << objectID << dec << "]\n";
			}*/
		}
	
		inline void removeInRangeObject(int index) {
			closeobjects.remove(index);
		}
	
		inline void removeInRangeObjects() {
			closeobjects.removeAll();
		}
	
		bool isInRange(QuadTreeEntry* obj, float range) {
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
	
		int compareTo(QuadTreeEntry* obj) {
			if (objectID < obj->objectID)
				return 1;
			else if (objectID > obj->objectID)
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
	
		// setters and getters
		inline void setObjectID(uint64 id) {
			objectID = id;
		}
	
		inline uint64 getObjectID() {
			return objectID;
		}
	
		friend class QuadTree;	
		friend class QuadTreeNode;
	};

  } // namespace util
} // namespace engine

using namespace engine::util;

#endif /*QUADTREEENTRY_H_*/
