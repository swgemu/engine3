/*
 * DOBObjectManagerImplementation.cpp
 *
 *  Created on: 22/08/2009
 *      Author: victor
 */

#include "DOBObjectManagerImplementation.h"

DistributedObjectAdapter* DOBObjectManagerImplementation::addObject(DistributedObjectStub* object) {
	DistributedObjectServant* servant = object->_getImplementation();

	if (servant == NULL) {
		// object not local we add it to remote DOB <-> hosts map
		return NULL;
	}

	DistributedObjectClassHelper* helper = servant->_getClassHelper();
	DistributedObjectAdapter* adapter = helper->createAdapter(object);

	return localObjectDirectory.add(object->_getObjectID(), adapter);
}

DistributedObject* DOBObjectManagerImplementation::getObject(uint64 objectID, bool doLock) {
	DistributedObject* obj = NULL;

	lock(doLock);

	obj = localObjectDirectory.get(objectID);

	unlock(doLock);

	if (obj != NULL)
		return obj;
	else {
		//get the object from the remote DOB map
	}

	return obj;
}

