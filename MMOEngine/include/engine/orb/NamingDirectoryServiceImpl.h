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
	
		uint64 nextObjectID;
		
	public:
		NamingDirectoryServiceImpl();
		NamingDirectoryServiceImpl(const string& address);
	
		void deploy(DistributedObjectStub* stub);
		void deploy(const string& name, DistributedObjectStub* stub);
	
		DistributedObject* lookUp(const string& name);
		
		DistributedObject* undeploy(const string& name);
		
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
