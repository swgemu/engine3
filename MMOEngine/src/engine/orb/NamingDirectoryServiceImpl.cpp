/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DistributedObjectBroker.h"

#include "NamingDirectoryServiceImpl.h"

NamingDirectoryServiceImpl::NamingDirectoryServiceImpl()
		: NamingDirectoryService() {
	rootNamingDirectory = NULL;
}

NamingDirectoryServiceImpl::NamingDirectoryServiceImpl(const String& address)
		: NamingDirectoryService() {
	rootNamingDirectory = new NamingDirectoryService(address);
}

void NamingDirectoryServiceImpl::deploy(DistributedObjectStub* stub) {
	String name = stub->_getName();
	uint64 objectid = stub->_getObjectID();

	if (name.isEmpty()) {
		StringBuffer orbname;
		orbname << "_OrbObject" << objectid;

		name = orbname.toString();

		stub->_setName(name);
	}

	if (objectNameMap.put(name, stub) != NULL)
		DistributedObjectBroker::instance()->error("object \'" + name + "\' already deployed");
}

void NamingDirectoryServiceImpl::deploy(const String& name, DistributedObjectStub* stub) {
	if (objectNameMap.put(name, stub) != NULL)
		DistributedObjectBroker::instance()->error("object \'" + name + "\' already deployed");

	stub->_setName(name);
}

DistributedObject* NamingDirectoryServiceImpl::lookUp(const String& name) {
	return objectNameMap.get(name);
}

DistributedObject* NamingDirectoryServiceImpl::undeploy(const String& name) {
	DistributedObject* obj = objectNameMap.get(name);

	if (obj != NULL)
		objectNameMap.remove(name);

	return obj;
}
