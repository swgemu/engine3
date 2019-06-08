/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/db/ObjectDatabaseManager.h"
#include "engine/db/mysql/MysqlDatabaseManager.h"

#include "TransactionalObjectManager.h"

#include "engine/core/ManagedObject.h"

TransactionalObjectManager::TransactionalObjectManager() : Command() {
	objectBroker = DistributedObjectBroker::instance();
}

void TransactionalObjectManager::initialize() {
	ObjectDatabaseManager::instance()->startLocalTransaction();
}

void TransactionalObjectManager::execute() {
	LocalObjectManager* manager = getLocalObjectManager();

	manager->commitObjectChanges();

	ObjectDatabaseManager::instance()->commitLocalTransaction();

	MysqlDatabaseManager::instance()->commitModifiedDatabases();
}

void TransactionalObjectManager::undo() {
	LocalObjectManager* manager = getLocalObjectManager();

	manager->clearObjectChanges();

	ObjectDatabaseManager::instance()->abortLocalTransaction();

	MysqlDatabaseManager::instance()->rollbackModifiedDatabases();
}

ObjectsToSaveMap* TransactionalObjectManager::getModifiedObjectsToSave() {
	Locker locker(&saveMutex);

	ObjectsToSaveMap* copy = new ObjectsToSaveMap(implementationCopiesToSave);
	implementationCopiesToSave.removeAll();

	return copy;
}

void TransactionalObjectManager::addObjectsToSave(const Vector<Reference<Object*> >& objects) {

	return;
/*
	Locker locker(&saveMutex);

	for (int i = 0; i < objects.size(); ++i) {
		ManagedObjectImplementation* impl = dynamic_cast<ManagedObjectImplementation*>(objects.get(i).get());

		//Di

		if (impl != nullptr && (impl->_this != nullptr)) {
			implementationCopiesToSave.put(impl->_this.get(), impl);
		}
	}*/
}

void TransactionalObjectManager::registerClass(const String& name, DistributedObjectClassHelper* helper) {
	objectBroker->registerClass(name, helper);
}

void TransactionalObjectManager::deploy(DistributedObjectStub* obj) {
	LocalObjectManager* manager = getLocalObjectManager();

	manager->deploy(obj);
}

void TransactionalObjectManager::deploy(const String& name, DistributedObjectStub* obj) {
	LocalObjectManager* manager = getLocalObjectManager();

	manager->deploy(name, obj);
}

DistributedObjectStub* TransactionalObjectManager::undeploy(const String& name) {
	LocalObjectManager* manager = getLocalObjectManager();

	return manager->undeploy(name);
}

Reference<DistributedObject*> TransactionalObjectManager::lookUp(const String& name) {
	LocalObjectManager* manager = getLocalObjectManager();

	return manager->lookUp(name);
}

Reference<DistributedObject*> TransactionalObjectManager::lookUp(uint64 objid) {
	LocalObjectManager* manager = getLocalObjectManager();

	return manager->lookUp(objid);
}

bool TransactionalObjectManager::destroyObject(DistributedObjectStub* obj) {
	LocalObjectManager* manager = getLocalObjectManager();

	return manager->destroyObject(obj);
}

LocalObjectManager* TransactionalObjectManager::getLocalObjectManager() {
	LocalObjectManager* manager = localObjectManager.get();
	if (manager == nullptr) {
		manager = new LocalObjectManager();

		localObjectManager.set(manager);
	}

	return manager;
}
