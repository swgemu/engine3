/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DistributedObjectBroker.h"

#include "NamingDirectoryService.h"

#include "packets/LookUpObjectMessage.h"
#include "packets/LookUpObjectResponseMessage.h"

NamingDirectoryService::NamingDirectoryService() {
	brokerClient = NULL;

	classMap = DistributedObjectBroker::instance()->getClassMap();
}

NamingDirectoryService::NamingDirectoryService(const String& address) {
	brokerClient = new DistributedObjectBrokerClient(DistributedObjectBroker::instance(), address);

	classMap = DistributedObjectBroker::instance()->getClassMap();
}

NamingDirectoryService::~NamingDirectoryService() {
}

void NamingDirectoryService::deploy(DistributedObjectStub* stub) {
}

void NamingDirectoryService::deploy(const String& name, DistributedObjectStub* stub) {
}

DistributedObject* NamingDirectoryService::lookUp(const String& name) {
	Packet* msg = new LookUpObjectMessage(name);
	brokerClient->send(msg);
			
	Packet resp;
	brokerClient->read(&resp);
	
	if (!LookUpObjectResponseMessage::parseObjectFound(&resp))
		return NULL;
			
	String className;
	resp.parseAscii(className);
	
	uint64 objectid = resp.parseLong();

	DistributedObject* obj = NULL;

	DistributedObjectClassHelper* helper = classMap->get(className);
	if (helper != NULL) {
		obj = helper->instantiateObject();
				
		obj->_setName(name);
		obj->_setObjectID(objectid);			
				
		obj->_setNamingDirectory(this);
	}		
	 
	return obj;
}

DistributedObject* NamingDirectoryService::undeploy(const String& name) {
	DistributedObject* obj = NULL;
	
	return obj;
}
