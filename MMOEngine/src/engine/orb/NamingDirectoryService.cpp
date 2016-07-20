/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DistributedObjectBroker.h"

NamingDirectoryService::NamingDirectoryService() {
}

NamingDirectoryService::~NamingDirectoryService() {
}

bool NamingDirectoryService::bind(const String& name, DistributedObjectStub* stub) {
	if (objectNameMap.put(name, stub) != NULL)
		return false;

	stub->_setName(name);

	return true;
}

DistributedObject* NamingDirectoryService::lookup(const String& name) {
	return objectNameMap.get(name);
}

DistributedObject* NamingDirectoryService::unbind(const String& name) {
	DistributedObject* obj = objectNameMap.get(name);

	if (obj != NULL)
		objectNameMap.remove(name);

	return obj;
}
