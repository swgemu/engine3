/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/
#include "ref/WeakReference.h"

#include "../thread/Locker.h"

#include "Object.h"

void Object::acquireWeak(WeakReference* ref) {
	Locker locker(referenceMutex);

	weakReferences.add(ref);
}

void Object::release() {
	if (decreaseCount())
		destroy();
}

void Object::releaseWeak(WeakReference* ref) {
	Locker locker(referenceMutex);

	weakReferences.remove(ref);
}

void Object::destroy() {
	Locker locker(referenceMutex);

	while (!weakReferences.isEmpty()) {
		WeakReference* ref = weakReferences.get(i);
		ref->releaseObject();
	}

	locker.release();

	delete this;
}

#ifdef TRACE_REFERENCES
void Object::addHolder(void* obj) {
	Locker locker(referenceMutex);

	StackTrace* trace = new StackTrace();
	referenceHolders.put(obj, trace);
}

void Object::removeHolder(void* obj) {
	Locker locker(referenceMutex);

	StackTrace* trace = referenceHolders.get(obj);

	if (trace != NULL) {
		delete trace;
		referenceHolders.drop(obj);
	}
}

void Object::printReferenceHolders() {
	for (int i = 0; i < referenceHolders.size(); ++i) {
		StackTrace* trace = referenceHolders.get(i);

		trace->print();
	}
}
#endif
