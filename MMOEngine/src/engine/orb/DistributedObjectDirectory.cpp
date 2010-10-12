/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DistributedObjectDirectory.h"
#include "../core/ManagedObject.h"
#include "../log/Logger.h"
#include "../db/ObjectDatabaseManager.h"

DistributedObjectDirectory::DistributedObjectDirectory() {
}

DistributedObjectAdapter* DistributedObjectDirectory::add(uint64 objid, DistributedObjectAdapter* adapter) {
	return objectMap.put(objid, adapter);
}

DistributedObject* DistributedObjectDirectory::get(uint64 objid) {
	DistributedObjectAdapter* adapter = NULL;

	try {
		adapter = objectMap.get(objid);
	} catch (...) {

	}

	if (adapter != NULL)
		return adapter->getStub();
	else 
		return NULL;
}

DistributedObjectAdapter* DistributedObjectDirectory::remove(uint64 objid) {
	DistributedObjectAdapter* adapter = objectMap.get(objid);

	if (adapter != NULL)
		objectMap.remove(objid);
	
	return adapter;
}

DistributedObjectAdapter* DistributedObjectDirectory::getAdapter(uint64 objid) {
	return objectMap.get(objid);
}

void DistributedObjectDirectory::destroyContainingObjects() {
	//while (objectMap.)
	HashTableIterator<uint64, DistributedObjectAdapter*> iterator(&objectMap);
	Vector<Reference<DistributedObject*> > objects(objectMap.size(), 1);

	int i = 0;

	while (iterator.hasNext()) {
		DistributedObjectAdapter* adapter = iterator.getNextValue();

		DistributedObject* dobObject = adapter->getStub();

		objects.add(dobObject);
	}

	savePersistentObjects();

	objectMap.removeAll();
}

void DistributedObjectDirectory::savePersistentObjects() {
	Logger::console.info("[DistributedObjectDirectory] saving persistent objects ", true);

	//System::out << "[DistributedObjectDirectory] saving persistent objects " << endl;

	int i = 0;

	HashTableIterator<uint64, DistributedObjectAdapter*> iterator(&objectMap);

	while (iterator.hasNext()) {
		DistributedObjectAdapter* adapter = iterator.getNextValue();

		DistributedObject* dobObject = adapter->getStub();

		ManagedObject* managedObject = dynamic_cast<ManagedObject*>(dobObject);

		if (managedObject != NULL && managedObject->isPersistent()) {
			Locker locker(managedObject);

			managedObject->updateToDatabase();
		}

		++i;

		if (i % 512 == 0)
			ObjectDatabaseManager::instance()->commitLocalTransaction();
	}

	ObjectDatabaseManager::instance()->commitLocalTransaction();

	ObjectDatabaseManager::instance()->checkpoint();

	StringBuffer msg;
	msg << "[DistributedObjectDirectory] finished saving " << i << " persistent objects " << endl;
	Logger::console.info(msg.toString(), true);
}
