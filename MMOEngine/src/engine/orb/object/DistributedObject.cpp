/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/core/Core.h"
#include "engine/orb/messages/DOBServiceClient.h"

#include "DistributedObject.h"

#include "engine/orb/db/DOBObjectManager.h"

DistributedObject::DistributedObject() : Object(), _objectID(0), _objectBroker(nullptr),
	_deletedFromDatabase(false) {

	_markedForDeletion.obj = this;
	_markedForDeletion._delete = false;

	_updated.obj = this;
	_updated._updated = false;
}

DistributedObject::~DistributedObject() {
}

DistributedObject::UpdatedHelper::~UpdatedHelper() {
	auto trace = lastModifiedTrace.get();

	if (trace) {
		delete trace;
	}
}

void DistributedObject::UpdatedHelper::clearTrace() {
	auto oldTrace = lastModifiedTrace.get();

	if (oldTrace && lastModifiedTrace.compareAndSet(oldTrace, nullptr)) {
		delete oldTrace;
	}
}

//dirty impl until i modify idlc
DistributedObject::UpdatedHelper& DistributedObject::UpdatedHelper::operator=(bool val) {
	_updated.store(val, std::memory_order_relaxed);

	if (!val) {
		clearTrace();

		return *this;
	}

	/*if (!obj->isPersistent())
		return *this;*/

	const bool toggle = DOBObjectManager::getDumpLastModifiedTraces();

	if (toggle) {
		clearTrace();

		auto newTrace = new StackTrace();

		if (!lastModifiedTrace.compareAndSet(nullptr, newTrace)) {
			delete newTrace;
		}
	}

	const static bool enabled = Core::getIntProperty("ObjectManager.saveMode", 0);

	if (!enabled)
		return *this;

	auto thread = Thread::getCurrentThread();

	if (!thread) {
		Logger::console.warning("null current thread in managed object updated mark");

		return *this;
	}

	thread->addModifiedObject(obj);

	return *this;
}

//dirty impl until i modify idlc
DistributedObject::DeleteHelper& DistributedObject::DeleteHelper::operator=(bool val) {
	_delete.store(val, std::memory_order_relaxed);

	if (!val) {
		return *this;
	}

	const static bool enabled = Core::getIntProperty("ObjectManager.saveMode", 0);

	if (!enabled)
		return *this;

	auto thread = Thread::getCurrentThread();

	if (!thread) {
		Logger::console.warning("null current thread in managed object delete mark");

		return *this;
	}

	thread->addDeleteFromDatabaseObject(obj);

	return *this;
}



