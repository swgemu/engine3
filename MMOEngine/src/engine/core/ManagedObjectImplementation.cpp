#include "ManagedObject.h"

void ManagedObject::lock(bool doLock) {
	DistributedObjectStub::wlock(doLock);
	
	if (_impl == NULL)
		_wlock(doLock);
}

void ManagedObject::lock(ManagedObject* obj) {
	DistributedObjectStub::wlock(obj);

	if (_impl == NULL)
		_wlock(obj);
}

void ManagedObject::wlock(bool doLock) {
	DistributedObjectStub::wlock(doLock);

	if (_impl == NULL)
		_wlock(doLock);
}

void ManagedObject::wlock(ManagedObject* obj) {
	DistributedObjectStub::wlock(obj);

	if (_impl == NULL)
		_wlock(obj);
}

void ManagedObject::unlock(bool doLock) {
	DistributedObjectStub::unlock(doLock);

	if (_impl == NULL)
		_unlock(doLock);
}

void ManagedObject::setLockName(const String& name) {
	DistributedObjectStub::setLockName(name);

	if (_impl == NULL)
		_setLockName(name);
}

void ManagedObjectImplementation::lock(bool doLock) {
	_this->wlock(doLock);
}

void ManagedObjectImplementation::lock(ManagedObject* obj) {
	_this->wlock(obj);
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

void ManagedObjectImplementation::setLockName(const String& name) {
	_this->setLockName(name);
}
