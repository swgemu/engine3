/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DistributedObjectDirectory.h"
#include "../core/ManagedObject.h"
#include "../log/Logger.h"
#include "../db/ObjectDatabaseManager.h"
#include "DistributedHelperObjectMap.h"

DistributedObjectDirectory::DistributedObjectDirectory() {
	helperObjectMap = new DistributedHelperObjectMap();
}

DistributedObjectDirectory::~DistributedObjectDirectory() {
	//delete helperObjectMap;
	helperObjectMap = NULL;
}

DistributedObjectAdapter* DistributedObjectDirectory::add(uint64 objid, DistributedObjectAdapter* adapter) {
	helperObjectMap->put(objid, adapter->getStub());

	return objectMap.put(objid, adapter);
}

DistributedObject* DistributedObjectDirectory::get(uint64 objid) {
	DistributedObjectAdapter* adapter = NULL;

	try {
		adapter = objectMap.get(objid);
	} catch (...) {

	}

	if (adapter != NULL) {
		helperObjectMap->put(objid, adapter->getStub());
		return adapter->getStub();
	} else
		return NULL;
}

DistributedObjectAdapter* DistributedObjectDirectory::remove(uint64 objid) {
	DistributedObjectAdapter* adapter = objectMap.get(objid);

	if (adapter != NULL)
		objectMap.remove(objid);
	
	helperObjectMap->remove(objid);

	return adapter;
}

DistributedObjectAdapter* DistributedObjectDirectory::getAdapter(uint64 objid) {
	return objectMap.get(objid);
}

void DistributedObjectDirectory::getObjectsMarkedForUpdate(Vector<DistributedObject*>& objectsToUpdate, Vector<DistributedObject*>& objectsToDelete,
		Vector<Reference<DistributedObject*> >& objectsToDeleteFromRAM) {

	objectsToUpdate.removeAll(objectMap.size(), 1);

	HashTableIterator<uint64, DistributedObjectAdapter*> iterator(&objectMap);

	while (iterator.hasNext()) {
		DistributedObjectAdapter* adapter = iterator.getNextValue();

		DistributedObject* dobObject = adapter->getStub();
		DistributedObjectServant* dobServant = adapter->getImplementation();

		if (dobObject->getReferenceCount() == 1)
			objectsToDeleteFromRAM.add(dobObject);

		ManagedObject* managedObject = dynamic_cast<ManagedObject*>(dobObject);

		if (managedObject == NULL/* || !managedObject->isPersistent()*/)
			continue;

		if (dobObject->_isMarkedForDeletion()) {
			objectsToDelete.add(dobObject);
		} else if (dobObject->_isUpdated() && ((ManagedObjectImplementation*)dobServant)->isPersistent()) {
			objectsToUpdate.add(dobObject);
		}
	}

	StringBuffer msg;
	msg << "[DistributedObjectDirectory] marked " << objectsToUpdate.size() << " objects to update and "
			<< objectsToDelete.size() << " for deletion";

	Logger::console.info(msg.toString(), true);
}
