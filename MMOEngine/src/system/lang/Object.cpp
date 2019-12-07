/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "ref/Reference.h"

#include "engine/core/ManagedReference.h"

#ifdef TRACE_REFERENCES
AtomicInteger ReferenceIdCounter::nextID;
#endif

Object::Object() : Variable() {
#ifdef MEMORY_PROTECTION
	_destroying = new AtomicBoolean(false);
#else
#endif

	referenceCounters = nullptr;

	//MemoryManager::getInstance()->create(this);

#ifdef TRACE_REFERENCES
	/*referenceHolders = new VectorMap<uint64, StackTrace*>();
	referenceHolders->setNullValue(nullptr);*/
	referenceHolders = nullptr;
#endif
}

Object::Object(const Object& obj) : Variable() {
#ifdef MEMORY_PROTECTION
	_destroying = new AtomicBoolean(false);
#else
#endif

	referenceCounters = nullptr;

	//MemoryManager::getInstance()->create(this);

#ifdef TRACE_REFERENCES
	/*referenceHolders = new VectorMap<uint64, StackTrace*>();
	referenceHolders->setNullValue(nullptr);*/
	referenceHolders = nullptr;
#endif
}

Object::~Object() NO_THREAD_SAFETY_ANALYSIS {
#ifdef TRACE_REFERENCES
	if (referenceHolders != nullptr) {
		for (int i = 0; i < referenceHolders->size(); ++i)
			delete referenceHolders->get(i);
	}
#endif

	auto counters = referenceCounters;

	if (counters != nullptr) {
		E3_ASSERT((counters->getStrongReferenceCount() % 2) != 0);

		if (counters->decrementAndTestAndSetWeakCount() != 0) {
			referenceCounters = nullptr;

			delete counters;
			//referenceCounters = nullptr;
		}

	}

#ifdef MEMORY_PROTECTION
	delete _destroying;
	_destroying = nullptr;
#endif

	finalize();

#ifdef TRACE_REFERENCES
	delete referenceHolders;
	referenceHolders = nullptr;
#endif

	//deletedByTrace = new StackTrace();
}

/*
bool Object::tryAcquire() {
	WMB();

	if (_destroying && getReferenceCount() == 0) {
		printf("try Acquire failed\n");

		return false;
	} else
		increaseCount();

	return true;
}
*/
/*
void Object::acquire() {
	if (referenceCounters == nullptr) {
		StrongAndWeakReferenceCount* newCount = new StrongAndWeakReferenceCount(0, 1);

		if (!referenceCounters.compareAndSet(nullptr, newCount)) {
			delete newCount;
		}
	}

	referenceCounters->increaseStrongCount();
}

void Object::release() {
	if (referenceCounters->decrementAndTestAndSetStrongCount() != 0) {
		if (notifyDestroy()) {
#ifdef WITH_STM
			MemoryManager::getInstance()->reclaim(this);
#else
			destroy();
#endif
		}
	}
}
*/
void Object::_destroyIgnoringCount() {
	if (notifyDestroy()) {
#ifdef WITH_STM
		MemoryManager::getInstance()->reclaim(this);
#else
		destroy();
#endif
	}
}

void Object::destroy() NO_THREAD_SAFETY_ANALYSIS {
#ifdef MEMORY_PROTECTION
	_destroying->set(true);
#else
#endif

	//printf("deleting in ::destroy\n");

#ifdef WITH_STM
	free();
#else
	Object::free();
#endif
}

void Object::free() {
	delete this;
}

#ifdef TRACE_REFERENCES
void Object::addHolder(uint64 obj) {
#ifndef WITH_STM
	Locker locker(&referenceMutex);
#endif

	StackTrace* trace = new StackTrace();

	if (referenceHolders == nullptr) {
		referenceHolders = new VectorMap<uint64, StackTrace*>();
		referenceHolders->setNullValue(nullptr);
	}

	referenceHolders->put(obj, trace);
}

void Object::removeHolder(uint64 obj) {
#ifndef WITH_STM
	Locker locker(&referenceMutex);
#endif
	if (referenceHolders == nullptr) {
		return;
	}

	StackTrace* trace = referenceHolders->get(obj);

	if (trace != nullptr) {
		delete trace;
		referenceHolders->drop(obj);
	}
}

void Object::printReferenceHolders() {
	if (referenceHolders == nullptr) {
		return;
	}

	for (int i = 0; i < referenceHolders->size(); ++i) {
		StackTrace* trace = referenceHolders->get(i);

		trace->print();
	}
}
#endif

String Object::toString() const {
	char buf[20];

	snprintf(buf, 20, "%p", reinterpret_cast<const void*>(this));

	return String(buf);
}
