/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef NAMINGDIRECTORYSERVICE_H_
#define NAMINGDIRECTORYSERVICE_H_

#include "messages/DOBServiceClient.h"

#include "object/DistributedObject.h"
#include "object/DistributedObjectClassHelperMap.h"

namespace engine {
  namespace ORB {

	class NamingDirectoryService {
		HashTable<String, DistributedObjectStub*> objectNameMap;

	public:
		NamingDirectoryService();
		
		virtual ~NamingDirectoryService();
	
		bool bind(const String& name, DistributedObjectStub* stub);
	
		DistributedObject* lookup(const String& name);
		
		DistributedObject* unbind(const String& name);
		
		HashTableIterator<String, DistributedObjectStub*> getBoundObjects() {
			return objectNameMap.iterator();
		}

		void clearNameMap() {
			objectNameMap.removeAll();
		}

	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*NAMINGDIRECTORYSERVICE_H_*/
