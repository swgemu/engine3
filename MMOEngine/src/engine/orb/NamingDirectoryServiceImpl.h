/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef NAMINGDIRECTORYSERVICEIMPL_H_
#define NAMINGDIRECTORYSERVICEIMPL_H_

#include "NamingDirectoryService.h"

namespace engine {
  namespace ORB {

	class NamingDirectoryServiceImpl : public NamingDirectoryService {
		NamingDirectoryService* rootNamingDirectory;

		HashTable<String, DistributedObjectStub*> objectNameMap;
		
	public:
		NamingDirectoryServiceImpl();
		NamingDirectoryServiceImpl(const String& address);
	
		bool bind(const String& name, DistributedObjectStub* stub);
	
		DistributedObject* lookup(const String& name);
		
		DistributedObject* unbind(const String& name);
		
		HashTableIterator<String, DistributedObjectStub*> getBoundObjects() {
			return objectNameMap.iterator();
		}

		void clearNameMap() {
			objectNameMap.removeAll();
		}

		inline bool isRootDirectory() {
			return rootNamingDirectory == NULL;
		}

		// setters
		inline void setRootNamingDirectory(NamingDirectoryService* directory) {
			rootNamingDirectory = directory;
		}

	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*NAMINGDIRECTORYSERVICEIMPL_H_*/
