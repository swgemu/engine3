/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ref/Reference.h"
#include "ref/WeakReference.h"

#include "engine/core/ManagedReference.h"

#include "system/thread/Locker.h"
#include "system/mm/MemoryManager.h"

#include "Object.h"

#include "system/util/SortedVector.h"
#include "system/util/HashSet.h"

#include "engine/stm/TransactionalReference.h"
#include "engine/stm/TransactionalObjectHeader.h"

#include "StackTrace.h"

#include "system/util/VectorMap.h"

#ifdef TRACE_REFERENCES
AtomicInteger ReferenceIdCounter::nextID;
#endif

Object::Object() : Variable() {
#ifdef MEMORY_PROTECTION
	_destroying = new AtomicBoolean(false);
#else
	_destroying = false;
#endif

	referenceCounters = NULL;

	//MemoryManager::getInstance()->create(this);

#ifdef TRACE_REFERENCES
	/*referenceHolders = new VectorMap<uint64, StackTrace*>();
	referenceHolders->setNullValue(NULL);*/
	referenceHolders = NULL;
#endif
}

Object::Object(const Object& obj) : Variable() {
#ifdef MEMORY_PROTECTION
	_destroying = new AtomicBoolean(false);
#else
	_destroying = false;
#endif

	referenceCounters = NULL;

	//MemoryManager::getInstance()->create(this);

#ifdef TRACE_REFERENCES
	/*referenceHolders = new VectorMap<uint64, StackTrace*>();
	referenceHolders->setNullValue(NULL);*/
	referenceHolders = NULL;
#endif
}

Object::~Object() {
#ifdef TRACE_REFERENCES
	if (referenceHolders != NULL) {
		for (int i = 0; i < referenceHolders->size(); ++i)
			delete referenceHolders->get(i);
	}
#endif

	if (referenceCounters != NULL) {
		if ((referenceCounters->getStrongReferenceCount() % 2) == 0) {
			assert(0 && "Deleting object with strong references");
		}

		if (referenceCounters->decrementAndTestAndSetWeakCount() != 0) {
			delete referenceCounters;
			referenceCounters = NULL;
		}

	}

#ifdef MEMORY_PROTECTION
	delete _destroying;
	_destroying = NULL;
#endif

	finalize();

#ifdef TRACE_REFERENCES
	delete referenceHolders;
	referenceHolders = NULL;
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
	if (referenceCounters == NULL) {
		StrongAndWeakReferenceCount* newCount = new StrongAndWeakReferenceCount(0, 1);

		if (!referenceCounters.compareAndSet(NULL, newCount)) {
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

void Object::destroy() {
#ifdef MEMORY_PROTECTION
	_destroying->set(true);
#else
	_destroying = true;

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

	if (referenceHolders == NULL) {
		referenceHolders = new VectorMap<uint64, StackTrace*>();
		referenceHolders->setNullValue(NULL);
	}

	referenceHolders->put(obj, trace);
}

void Object::removeHolder(uint64 obj) {
#ifndef WITH_STM
	Locker locker(&referenceMutex);
#endif
	if (referenceHolders == NULL) {
		return;
	}

	StackTrace* trace = referenceHolders->get(obj);

	if (trace != NULL) {
		delete trace;
		referenceHolders->drop(obj);
	}
}

void Object::printReferenceHolders() {
	if (referenceHolders == NULL) {
		return;
	}

	for (int i = 0; i < referenceHolders->size(); ++i) {
		StackTrace* trace = referenceHolders->get(i);

		trace->print();
	}
}
#endif

String Object::toString() {
	char buf[20];

	sprintf(buf, "%p", this);

	return String(buf);
}
