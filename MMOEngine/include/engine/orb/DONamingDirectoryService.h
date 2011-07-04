/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DOBNAMINGDIRECTORYSERVICE_H_
#define DOBNAMINGDIRECTORYSERVICE_H_

#include "DistributedObjectBrokerClient.h"

#include "object/DistributedObject.h"

#include "object/DistributedObjectNameMap.h"
#include "object/DistributedObjectClassHelperMap.h"

namespace engine {
  namespace ORB {

	class DistributedObjectBroker;
	
	class DONamingDirectoryService {
		DistributedObjectBrokerClient* rootNamingDirectory;
		
		DistributedObjectNameMap objectNameMap;
		DistributedObjectClassHelperMap* classMap;
	
		uint64 nextObjectID;
		
	public:
		DONamingDirectoryService(DistributedObjectBroker* broker);
		DONamingDirectoryService(DistributedObjectBroker* broker, const string& address);
	
		void deploy(DistributedObjectStub* stub);
		void deploy(const string& name, DistributedObjectStub* stub);
	
		DistributedObject* lookUp(const string& name);
		
		DistributedObject* undeploy(const string& name);
		
		inline bool isRootDirectory() {
			return rootNamingDirectory == NULL;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DOBNAMINGDIRECTORYSERVICE_H_*/
