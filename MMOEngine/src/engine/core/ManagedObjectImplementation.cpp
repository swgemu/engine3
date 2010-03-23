#include "ManagedObject.h"
#include "engine/engine.h"
#include "ObjectUpdateToDatabaseTask.h"

void ManagedObject::lock(bool doLock) {
	/*DistributedObjectStub::wlock(doLock);

	if (_impl == NULL)
		_wlock(doLock);*/
}

void ManagedObject::lock(ManagedObject* obj) {
	/*DistributedObjectStub::wlock(obj);

	if (_impl == NULL)
		_wlock(obj);*/
}

void ManagedObject::rlock(bool doLock) {
	/*DistributedObjectStub::rlock(doLock);

	if (_impl == NULL)
		_rlock(doLock);*/
}

void ManagedObject::wlock(bool doLock) {
	/*DistributedObjectStub::wlock(doLock);

	if (_impl == NULL)
		_wlock(doLock);*/
}

void ManagedObject::wlock(ManagedObject* obj) {
	/*DistributedObjectStub::wlock(obj);

	if (_impl == NULL)
		_wlock(obj);*/
}

void ManagedObject::unlock(bool doLock) {
	if (getPersistenceLevel() == 3)
		updateToDatabase();

	/*DistributedObjectStub::unlock(doLock);

	if (_impl == NULL)
		_unlock(doLock);*/
}

void ManagedObject::runlock(bool doLock) {
	/*DistributedObjectStub::runlock(doLock);

	if (_impl == NULL)
		_runlock(doLock);*/
}

void ManagedObject::setLockName(const String& name) {
	/*DistributedObjectStub::setLockName(name);

	if (_impl == NULL)
		_setLockName(name);*/
}

void ManagedObjectImplementation::lock(bool doLock) {
	_this->wlock(doLock);
}

void ManagedObjectImplementation::lock(ManagedObject* obj) {
	_this->wlock(obj);
}

void ManagedObjectImplementation::rlock(bool doLock) {
	_this->rlock(doLock);
}

void ManagedObjectImplementation::wlock(bool doLock) {
	_this->wlock(doLock);
}

void ManagedObjectImplementation::wlock(ManagedObject* obj) {
	_this->wlock(obj);
}

void ManagedObjectImplementation::unlock(bool doLock) {
	_this->unlock(doLock);
}

void ManagedObjectImplementation::runlock(bool doLock) {
	_this->runlock(doLock);
}

void ManagedObjectImplementation::setLockName(const String& name) {
	_this->setLockName(name);
}

void ManagedObjectImplementation::updateToDatabase() {
	if (persistenceLevel == 0)
		return;

	DOBObjectManager* objectManager = DistributedObjectBroker::instance()->getObjectManager();

	objectManager->updatePersistentObject(_this);

	queueUpdateToDatabaseTask();
}

void ManagedObjectImplementation::queueUpdateToDatabaseTask() {
	if (updateToDatabaseTask != NULL || persistenceLevel != 2)
		return;

	updateToDatabaseTask = new ObjectUpdateToDatabaseTask(_this);
	updateToDatabaseTask->schedule();
}

void ManagedObjectImplementation::setPersistent(int level) {
	persistenceLevel = level;

	queueUpdateToDatabaseTask();
}

void ManagedObjectImplementation::initializeTransientMembers() {
	if (persistenceLevel == 2)
		queueUpdateToDatabaseTask();
}
