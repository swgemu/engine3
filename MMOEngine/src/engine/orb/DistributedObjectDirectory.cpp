/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DistributedObjectDirectory.h"
#include "../core/ManagedObject.h"
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

void DistributedObjectDirectory::savePersistentObjects() {
	/*class DistributedObjectMap : public HashTable<uint64, DistributedObjectAdapter*> {


	};*/
	System::out << "[DistributedObjectDirectory] saving persistent objects " << endl;

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

	System::out << "[DistributedObjectDirectory] finished saving persistent objects " << endl;
}
