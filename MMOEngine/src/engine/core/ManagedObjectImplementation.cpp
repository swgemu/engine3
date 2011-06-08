#include "ManagedObject.h"
#include "engine/engine.h"
#include "ObjectUpdateToDatabaseTask.h"

void ManagedObject::updateForWrite() {
}

void ManagedObject::lock(bool doLock) {
#ifndef WITH_STM
	DistributedObjectStub::wlock(doLock);

	if (_getImplementation() == NULL)
		_wlock(doLock);
#endif
}

/*void ManagedObject::lock(Lockable* obj) {
#ifndef WITH_STM
	DistributedObjectStub::wlock(obj);

	if (_getImplementation() == NULL)
		_wlock(obj);
#endif
}*/

void ManagedObject::lock(ManagedObject* obj) {
#ifndef WITH_STM
DistributedObjectStub::wlock(obj);

	if (_getImplementation() == NULL)
		_wlock(obj);
#endif
}

void ManagedObject::rlock(bool doLock) {
#ifndef WITH_STM
	DistributedObjectStub::rlock(doLock);

	if (_getImplementation() == NULL)
		_rlock(doLock);
#endif
}

void ManagedObject::wlock(bool doLock) {
#ifndef WITH_STM
	DistributedObjectStub::wlock(doLock);

	if (_getImplementation() == NULL)
		_wlock(doLock);
#endif
}

void ManagedObject::wlock(ManagedObject* obj) {
#ifndef WITH_STM
	DistributedObjectStub::wlock(obj);

	if (_getImplementation() == NULL)
		_wlock(obj);
#endif
}

void ManagedObject::lock(Lockable* obj) {
#ifndef WITH_STM
	DistributedObjectStub::lock(obj);

	if (_getImplementation() == NULL)
		_lock(obj);
#endif
}

void ManagedObject::unlock(bool doLock) {
#ifndef WITH_STM
	if (getPersistenceLevel() == 3) // change to 3
		updateToDatabase();

	DistributedObjectStub::unlock(doLock);

	if (_getImplementation() == NULL)
		_unlock(doLock);
#endif
}

void ManagedObject::runlock(bool doLock) {
#ifndef WITH_STM
	DistributedObjectStub::runlock(doLock);

	if (_getImplementation() == NULL)
		_runlock(doLock);
#endif
}

void ManagedObject::setLockName(const String& name) {
	DistributedObjectStub::setLockName(name);

	if (_getImplementation() == NULL)
		_setLockName(name);
}

bool ManagedObject::notifyDestroy() {
	ObjectBroker* broker = Core::getObjectBroker();

	if (broker == NULL)
		return true;
	else
		return broker->destroyObject(this);
}

void ManagedObject::writeObject(ObjectOutputStream* stream) {
	Reference<ManagedObjectImplementation*> _implementation;
#ifdef WITH_STM
	_implementation = (ManagedObjectImplementation*) getForDirty();
#else
	_implementation = (ManagedObjectImplementation*) _getImplementation();
#endif

	if (_implementation == NULL) {
		throw ObjectNotLocalException(this);
	} else
		_implementation->writeObject(stream);
}

unsigned int ManagedObject::getLastCRCSave() {
	Reference<ManagedObjectImplementation*> _implementation;

#ifdef WITH_STM
	_implementation = (ManagedObjectImplementation*) getForDirty();
#else
	_implementation = (ManagedObjectImplementation*) _getImplementation();
#endif

	if (_implementation == NULL) {
		throw ObjectNotDeployedException(this);
	} else
		return _implementation->getLastCRCSave();
}

void ManagedObject::setLastCRCSave(unsigned int crc) {
	Reference<ManagedObjectImplementation*> _implementation;

#ifdef WITH_STM
	_implementation = (ManagedObjectImplementation*) getForDirty();
#else
	_implementation = (ManagedObjectImplementation*) _getImplementation();
#endif

	if (_implementation == NULL) {
		throw ObjectNotLocalException(this);
	} else
		_implementation->setLastCRCSave(crc);
}



bool ManagedObjectImplementation::notifyDestroy() {
	return _this->notifyDestroy();
}

void ManagedObjectImplementation::updateForWrite() {
}

void ManagedObjectImplementation::lock(bool doLock) {
	_this->wlock(doLock);
}

void ManagedObjectImplementation::lock(Lockable* obj) {
	_this->wlock(obj);
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
	//_this->runlock(doLock);
}

void ManagedObjectImplementation::setLockName(const String& name) {
	//_this->setLockName(name);
}

void ManagedObjectImplementation::updateToDatabase() {
	/*if (persistenceLevel == 0)
		return;

	DOBObjectManager* objectManager = DistributedObjectBroker::instance()->getObjectManager();

	objectManager->updatePersistentObject(_this);

	queueUpdateToDatabaseTask();*/
}

void ManagedObjectImplementation::queueUpdateToDatabaseTask() {
	/*if (updateToDatabaseTask != NULL || persistenceLevel != 2)
		return;

	updateToDatabaseTask = new ObjectUpdateToDatabaseTask(_this);
	updateToDatabaseTask->schedule();*/
}

void ManagedObjectImplementation::setPersistent(int level) {
	persistenceLevel = level;

	//queueUpdateToDatabaseTask();
}

void ManagedObjectImplementation::initializeTransientMembers() {
	if (persistenceLevel == 2)
		queueUpdateToDatabaseTask();

	lastCRCSave = 0;
}
