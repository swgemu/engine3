/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../DistributedObjectBroker.h"

#include "DistributedObjectStub.h"
#include "DistributedObjectServant.h"

DistributedObjectServant::DistributedObjectServant() {
}

DistributedObjectServant::~DistributedObjectServant() {
}

/*void DistributedObjectServant::lock(bool doLock) {
	_stub->lock(doLock);
}

void DistributedObjectServant::rlock(bool doLock) {
	_stub->rlock(doLock);
}

void DistributedObjectServant::wlock(bool doLock) {
	_stub->wlock(doLock);
}

void DistributedObjectServant::wlock(Mutex* lock) {
	_stub->wlock(lock);
}

void DistributedObjectServant::wlock(ReadWriteLock* lock) {
	_stub->wlock(lock);
}

void DistributedObjectServant::unlock(bool doLock) {
	_stub->unlock(doLock);
}

void DistributedObjectServant::runlock(bool doLock) {
	_stub->runlock(doLock);
}*/

void DistributedObjectServant::setDeployingName(const String& name) {
	_stub->_setName(name);
}
