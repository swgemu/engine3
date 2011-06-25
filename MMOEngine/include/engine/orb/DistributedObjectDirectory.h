/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DISTRIBUTEDOBJECTDIRECTORY_H_
#define DISTRIBUTEDOBJECTDIRECTORY_H_

#include "object/DistributedObject.h"
#include "object/DistributedObjectAdapter.h"

#include "engine/util/ObjectContainer.h"

namespace engine {
  namespace ORB {

	class DistributedObjectDirectory {
    	HashTable<uint64, DistributedObjectAdapter*> objectMap;

    	HashTable<uint64, Reference<DistributedObject*> >* helperObjectMap;
    	//HashTable<uint64, Reference<DistributedObject*> >* helperObjectMap;
	
	public:
		DistributedObjectDirectory();
		~DistributedObjectDirectory();
		
		DistributedObjectAdapter* add(sys::uint64 objid, DistributedObjectAdapter* adapter);
	
		DistributedObject* get(sys::uint64 objid);
		
		DistributedObjectAdapter* remove(sys::uint64 objid);

		void removeHelper(sys::uint64 objid);

		DistributedObjectAdapter* getAdapter(uint64 objid);

		void getObjectsMarkedForUpdate(Vector<DistributedObject*>& objectsToUpdate, Vector<DistributedObject*>& objectsToDelete, Vector<Reference<DistributedObject*> >& objectsToDeleteFromRAM);

		inline int getSize() {
			return objectMap.size();
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTDIRECTORY_H_*/
