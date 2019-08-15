/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/core/Core.h"
#include "engine/orb/messages/DOBServiceClient.h"

#include "DistributedObject.h"

DistributedObject::DistributedObject() : Object(), _objectID(0), _objectBroker(nullptr),
	_markedForDeletion(false), _deletedFromDatabase(false) {

	_updated.obj = this;
	_updated._updated = false;
}

DistributedObject::~DistributedObject() {
}

DistributedObject::UpdatedHelper::~UpdatedHelper() {
	if (lastModifiedTrace) {
		delete lastModifiedTrace;
	}
}

//dirty impl until i modify idlc
DistributedObject::UpdatedHelper& DistributedObject::UpdatedHelper::operator=(bool val) {
	_updated.store(val, std::memory_order_relaxed);

	if (!val)
		return *this;

	/*if (!obj->isPersistent())
		return *this;*/

	const static bool saveUpdatedTrace = Core::getIntProperty("ObjectManager.trackLastUpdatedTrace", 0);

	if (saveUpdatedTrace) {
		if (lastModifiedTrace) {
			delete lastModifiedTrace;
		}

		lastModifiedTrace = new StackTrace();
	}

	const static bool enabled = Core::getIntProperty("ObjectManager.saveMode", 0);

	if (!enabled)
		return *this;

	auto thread = Thread::getCurrentThread();

	if (!thread) {
		Logger::console.warning("null current thread in managed object unlock");

		return *this;
	}

	thread->addModifiedObject(obj);

	return *this;
}
