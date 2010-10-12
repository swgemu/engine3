/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DISTRIBUTEDOBJECTDIRECTORY_H_
#define DISTRIBUTEDOBJECTDIRECTORY_H_

#include "object/DistributedObject.h"
#include "object/DistributedObjectAdapter.h"

#include "object/DistributedObjectMap.h"

namespace engine {
  namespace ORB {

	class DistributedObjectDirectory {
		DistributedObjectMap objectMap;
	
	public:
		DistributedObjectDirectory();
		
		DistributedObjectAdapter* add(sys::uint64 objid, DistributedObjectAdapter* adapter);
	
		DistributedObject* get(sys::uint64 objid);
		
		DistributedObjectAdapter* remove(sys::uint64 objid);
	
		DistributedObjectAdapter* getAdapter(uint64 objid);

		void savePersistentObjects();
		void destroyContainingObjects();

		int getSize() {
			return objectMap.size();
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTDIRECTORY_H_*/
