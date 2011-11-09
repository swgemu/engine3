/*
 * DOBObjectManagerImplementation.cpp
 *
 *  Created on: 22/08/2009
 *      Author: victor
 */

#include "DOBObjectManagerImplementation.h"

DistributedObjectAdapter* DOBObjectManagerImplementation::addObject(DistributedObjectStub* object) {
	DistributedObjectServant* servant = object->getServant();

	if (servant == NULL) {
		// object not local we add it to remote DOB <-> hosts map
		return NULL;
	}

	DistributedObjectClassHelper* helper = servant->_getClassHelper();
	DistributedObjectAdapter* adapter = helper->createAdapter(object);

	return localObjectDirectory.add(object->_getObjectID(), adapter);
}

DistributedObject* DOBObjectManagerImplementation::getObject(uint64 objectID) {
	DistributedObject* obj = NULL;

	Locker _locker(this);

	obj = localObjectDirectory.get(objectID);

	if (obj != NULL)
		return obj;
	else {
		//get the object from the remote DOB map
	}

	return obj;
}

