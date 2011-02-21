/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DistributedObjectBroker.h"

#include "NamingDirectoryServiceImpl.h"

NamingDirectoryServiceImpl::NamingDirectoryServiceImpl()
		: NamingDirectoryService(), objectNameMap(300000) {
	rootNamingDirectory = NULL;
}

NamingDirectoryServiceImpl::NamingDirectoryServiceImpl(const String& address)
		: NamingDirectoryService() {
	rootNamingDirectory = new NamingDirectoryService(address);
}

bool NamingDirectoryServiceImpl::bind(const String& name, DistributedObjectStub* stub) {
	if (objectNameMap.put(name, stub) != NULL)
		return false;

	stub->_setName(name);

	return true;
}

DistributedObject* NamingDirectoryServiceImpl::lookup(const String& name) {
	return objectNameMap.get(name);
}

DistributedObject* NamingDirectoryServiceImpl::unbind(const String& name) {
	DistributedObject* obj = objectNameMap.get(name);

	if (obj != NULL)
		objectNameMap.remove(name);

	return obj;
}
