/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "DistributedObjectBroker.h"

NamingDirectoryService::NamingDirectoryService() {
}

NamingDirectoryService::~NamingDirectoryService() {
}

bool NamingDirectoryService::bind(const String& name, DistributedObjectStub* stub) {
	if (objectNameMap.put(name, stub) != nullptr)
		return false;

	stub->_setName(name);

	return true;
}

DistributedObject* NamingDirectoryService::lookup(const String& name) {
	return objectNameMap.get(name);
}

DistributedObject* NamingDirectoryService::unbind(const String& name) {
	DistributedObject* obj = objectNameMap.get(name);

	if (obj != nullptr)
		objectNameMap.remove(name);

	return obj;
}
