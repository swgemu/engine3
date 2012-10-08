/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "SynchronizedMessageQueue.h"

SynchronizedMessageQueue::SynchronizedMessageQueue(int capacity) : ArrayList<Message*>(capacity, 0), Logger("SynchronizedMessageQueue") {
	blocked = false;

	setLogging(false);
	setGlobalLogging(true);
}

SynchronizedMessageQueue::~SynchronizedMessageQueue() {
}

void SynchronizedMessageQueue::push(Message* msg) {
	if (blocked) {
		return;
	}

	uint32 index = writeIndex.increment() % elementCapacity;

	ArrayList<Message*>::set(index, msg);

	//signal(condMutex);
}

Message* SynchronizedMessageQueue::pop() {
	/*while (isEmpty()) {
		if (blocked) {
			return NULL;
		}

		wait(condMutex);
	}*/

	int index = readIndex.get();
	if (writeIndex.get() == index)
		return NULL;

	while (!readIndex.compareAndSet(index, index + 1)) {
		index = readIndex.get();
		if (writeIndex.get() == index)
			return NULL;
	}

	return ArrayList<Message*>::get(index);
}

void SynchronizedMessageQueue::flush() {
	blocked = true;

	//signal(condMutex);
}
