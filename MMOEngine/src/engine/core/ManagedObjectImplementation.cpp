/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include "ManagedObject.h"
#include "ObjectUpdateToDatabaseTask.h"
#include "engine/core/TaskWorkerThread.h"

void ManagedObject::updateForWrite() {
}

void ManagedObject::lock(bool doLock) ACQUIRE() {
#ifndef WITH_STM
	DistributedObjectStub::wlock(doLock);

	if (_getImplementationForRead() == nullptr)
		__wlock(doLock);
#endif
}

/*void ManagedObject::lock(Lockable* obj) {
#ifndef WITH_STM
	DistributedObjectStub::wlock(obj);

	if (_getImplementation() == nullptr)
		_wlock(obj);
#endif
}*/

void ManagedObject::lock(ManagedObject* obj) ACQUIRE() {
#ifndef WITH_STM
	DistributedObjectStub::wlock(obj);

	if (_getImplementationForRead() == nullptr)
		__wlock(obj);
#endif
}

void ManagedObject::rlock(bool doLock) ACQUIRE_SHARED() {
#ifndef WITH_STM
	DistributedObjectStub::rlock(doLock);

	if (_getImplementationForRead() == nullptr)
		__rlock(doLock);
#endif
}

void ManagedObject::wlock(bool doLock) ACQUIRE() {
#ifndef WITH_STM
	DistributedObjectStub::wlock(doLock);

	if (_getImplementationForRead() == nullptr)
		__wlock(doLock);
#endif
}

void ManagedObject::wlock(ManagedObject* obj) ACQUIRE() {
#ifndef WITH_STM
	DistributedObjectStub::wlock(obj);

	if (_getImplementationForRead() == nullptr)
		__wlock(obj);
#endif
}

void ManagedObject::lock(Lockable* obj) ACQUIRE() {
#ifndef WITH_STM
	DistributedObjectStub::lock(obj);

	if (_getImplementationForRead() == nullptr)
		__lock(obj);
#endif
}

void ManagedObject::unlock(bool doLock) RELEASE() {
#ifndef WITH_STM
	DistributedObjectStub::unlock(doLock);

	auto _implementation = static_cast<ManagedObjectImplementation*>(_getImplementationForRead());

	if (_implementation == nullptr) {
		__unlock(doLock);

		return;
	}

	/*
	if (!doLock) {
		return;
	}

	const static bool enabled = Core::getIntProperty("ObjectManager.saveMode", 0);

	if (!enabled || !_updated || !_implementation->isPersistent())
		return;

	auto thread = Thread::getCurrentThread();

	if (!thread) {
		Logger::console.warning("null current thread in managed object unlock");

		return;
	}

	thread->addModifiedObject(this);
	*/
#endif
}


void ManagedObject::rlock(ManagedObject* obj) ACQUIRE_SHARED() {
#ifndef WITH_STM
	DistributedObjectStub::rlock(obj);

	if (_getImplementationForRead() == nullptr)
		__rlock();
#endif
}

void ManagedObject::rlock(Lockable* obj) ACQUIRE_SHARED() {
#ifndef WITH_STM
	DistributedObjectStub::rlock(obj);

	if (_getImplementationForRead() == nullptr)
		__rlock();
#endif
}

void ManagedObject::runlock(bool doLock) RELEASE_SHARED() {
#ifndef WITH_STM
	DistributedObjectStub::runlock(doLock);

	if (_getImplementationForRead() == nullptr)
		__runlock(doLock);
#endif
}

void ManagedObject::setLockName(const String& name) {
	DistributedObjectStub::setLockName(name);

	if (_getImplementationForRead() == nullptr)
		__setLockName(name);
}

bool ManagedObject::notifyDestroy() {
	ObjectBroker* broker = Core::getObjectBroker();

	if (broker == nullptr)
		return true;
	else
		return broker->destroyObject(this);
}

void ManagedObject::writeObject(ObjectOutputStream* stream) {
	Reference<ManagedObjectImplementation*> _implementation;
#ifdef WITH_STM
	_implementation = header->getForDirty();
#else
	_implementation = (ManagedObjectImplementation*) _getImplementationForRead();
#endif

	if (_implementation == nullptr) {
		throw ObjectNotLocalException(this);
	} else
		_implementation->writeObject(stream);
}

void ManagedObject::writeJSON(JSONSerializationType& j) {
	Reference<ManagedObjectImplementation*> _implementation;
#ifdef WITH_STM
	_implementation = header->getForDirty();
#else
	_implementation = (ManagedObjectImplementation*) _getImplementationForRead();
#endif

	if (_implementation == nullptr) {
		throw ObjectNotLocalException(this);
	} else
		_implementation->writeJSON(j);
}

DistributedObjectServant* ManagedObject::getServant() {
#ifdef WITH_STM
	return header->getForDirty();
#else
	return _getImplementationForRead();
#endif
}

DistributedObjectServant* ManagedObjectImplementation::getServant() {
	return this;
}

bool ManagedObjectImplementation::notifyDestroy() {
	//return _this->notifyDestroy();
	return true;
}

void ManagedObjectImplementation::updateForWrite() {
}

void ManagedObjectImplementation::lock(bool doLock) NO_THREAD_SAFETY_ANALYSIS {
	_this.getReferenceUnsafeStaticCast()->wlock(doLock);
}

void ManagedObjectImplementation::lock(Lockable* obj) NO_THREAD_SAFETY_ANALYSIS {
	_this.getReferenceUnsafeStaticCast()->wlock(obj);
}

void ManagedObjectImplementation::lock(ManagedObject* obj) NO_THREAD_SAFETY_ANALYSIS {
	_this.getReferenceUnsafeStaticCast()->wlock(obj);
}

void ManagedObjectImplementation::rlock(Lockable* obj) NO_THREAD_SAFETY_ANALYSIS {
	_this.getReferenceUnsafeStaticCast()->rlock(obj);
}

void ManagedObjectImplementation::rlock(ManagedObject* obj) NO_THREAD_SAFETY_ANALYSIS {
	_this.getReferenceUnsafeStaticCast()->rlock(obj);
}

void ManagedObjectImplementation::rlock(bool doLock) NO_THREAD_SAFETY_ANALYSIS {
	_this.getReferenceUnsafeStaticCast()->rlock(doLock);
}

void ManagedObjectImplementation::wlock(bool doLock) NO_THREAD_SAFETY_ANALYSIS {
	_this.getReferenceUnsafeStaticCast()->wlock(doLock);
}

void ManagedObjectImplementation::wlock(ManagedObject* obj) NO_THREAD_SAFETY_ANALYSIS {
	_this.getReferenceUnsafeStaticCast()->wlock(obj);
}

void ManagedObjectImplementation::unlock(bool doLock) NO_THREAD_SAFETY_ANALYSIS {
	_this.getReferenceUnsafeStaticCast()->unlock(doLock);
}

void ManagedObjectImplementation::runlock(bool doLock) NO_THREAD_SAFETY_ANALYSIS {
	//_this->runlock(doLock);
	_this.getReferenceUnsafeStaticCast()->runlock(doLock);
}

void ManagedObjectImplementation::setLockName(const String& name) {
	//_this->setLockName(name);
}

void ManagedObjectImplementation::updateToDatabase() {
}

void ManagedObjectImplementation::setPersistent(int level) {
	persistenceLevel = level;

	_this.getReferenceUnsafeStaticCast()->_setUpdated(true);
}

void ManagedObjectImplementation::initializeTransientMembers() {
	lastSaveTime = 0;
	lastCRCSave = 0;
}
