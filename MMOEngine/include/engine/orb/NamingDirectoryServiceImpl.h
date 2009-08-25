/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef NAMINGDIRECTORYSERVICEIMPL_H_
#define NAMINGDIRECTORYSERVICEIMPL_H_

#include "NamingDirectoryService.h"

#include "object/DistributedObjectNameMap.h"

namespace engine {
  namespace ORB {

	class NamingDirectoryServiceImpl : public NamingDirectoryService {
		NamingDirectoryService* rootNamingDirectory;

		DistributedObjectNameMap objectNameMap;
		
	public:
		NamingDirectoryServiceImpl();
		NamingDirectoryServiceImpl(const String& address);
	
		void deploy(DistributedObjectStub* stub);
		void deploy(const String& name, DistributedObjectStub* stub);
	
		DistributedObject* lookUp(const String& name);
		
		DistributedObject* undeploy(const String& name);
		
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
