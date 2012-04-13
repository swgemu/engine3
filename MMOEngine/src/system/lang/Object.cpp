/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ref/Reference.h"
#include "ref/WeakReference.h"

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

Object::Object() : ReferenceCounter(), Variable() {
#ifdef MEMORY_PROTECTION
	_destroying = new AtomicBoolean(false);
#endif

#ifdef ENABLE_WEAK_REFS
#ifdef REFERENCED_WEAK_MUTEX
	referenceMutex = new Mutex();
#endif

	weakReferences = NULL;

#endif

	//MemoryManager::getInstance()->create(this);

#ifdef TRACE_REFERENCES
	/*referenceHolders = new VectorMap<uint64, StackTrace*>();
	referenceHolders->setNullValue(NULL);*/
	referenceHolders = NULL;
#endif
}

Object::Object(const Object& obj) : ReferenceCounter(), Variable() {
#ifdef MEMORY_PROTECTION
	_destroying = new AtomicBoolean(false);
#endif

#ifdef ENABLE_WEAK_REFS
#ifdef REFERENCED_WEAK_MUTEX
	referenceMutex = new Mutex();
#endif
	weakReferences = NULL;
#endif

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

	if (getReferenceCount() > 0)
		assert(0 && "Deleting object with reference > 0");

#ifdef ENABLE_WEAK_REFS
	if (weakReferences != NULL) {
#ifdef REFERENCED_WEAK_MUTEX
		Locker locker(referenceMutex);
#else
		Locker locker(&referenceMutex);
#endif
		HashSetIterator<WeakReferenceBase*> iterator = weakReferences->iterator();

		while (iterator.hasNext())
			iterator.getNextKey()->clearObject();

		delete weakReferences;
		weakReferences = NULL;

		locker.release();
	}
	

#ifdef REFERENCED_WEAK_MUTEX
	delete referenceMutex;
	referenceMutex = NULL;
#endif

#endif

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

void Object::release() {
/*	if (getReferenceCount() == 0)
		assert(0 && "Object already delted");*/

	if (decreaseCount()) {
		if (notifyDestroy()) {
#ifdef WITH_STM
			MemoryManager::getInstance()->reclaim(this);
#else
			destroy();
#endif
		}
	}
}

void Object::acquireWeak(WeakReferenceBase* ref) {
#ifdef ENABLE_WEAK_REFS
#ifdef REFERENCED_WEAK_MUTEX
	Locker locker(referenceMutex);
#else
	Locker locker(&referenceMutex);
#endif

	if (weakReferences == NULL)
		weakReferences = new HashSet<WeakReferenceBase*>();

	weakReferences->add(ref);
#endif
}

void Object::releaseWeak(WeakReferenceBase* ref) {
#ifdef ENABLE_WEAK_REFS
#ifdef REFERENCED_WEAK_MUTEX
	Locker locker(referenceMutex);
#else
	Locker locker(&referenceMutex);
#endif

	if (weakReferences == NULL)
		return;

	weakReferences->remove(ref);
#endif
}

void Object::destroy() {
#ifdef MEMORY_PROTECTION
	_destroying->set(true);
#else
	_destroying.set(true);
#endif
#ifdef ENABLE_WEAK_REFS
#ifdef REFERENCED_WEAK_MUTEX
	Locker locker(referenceMutex);
#else
	Locker locker(&referenceMutex);
#endif

	if (weakReferences != NULL) {
		HashSetIterator<WeakReferenceBase*> iterator = weakReferences->iterator();

		while (iterator.hasNext())
			iterator.getNextKey()->clearObject();

		delete weakReferences;
		weakReferences = NULL;
	}


	locker.release();
#endif

	if (getReferenceCount() > 0)
		assert(0 && "Deleting object with reference > 0");

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
