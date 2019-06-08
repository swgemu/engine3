/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

/*SynchronizedMessageQueue::SynchronizedMessageQueue(int capacity) : ArrayList<Message*>(capacity, 0), Logger("SynchronizedMessageQueue") {
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
	int index = readIndex.get();
	if (writeIndex.get() == index)
		return nullptr;

	while (!readIndex.compareAndSet(index, index + 1)) {
		index = readIndex.get();
		if (writeIndex.get() == index)
			return nullptr;
	}

	return ArrayList<Message*>::get(index);
}

void SynchronizedMessageQueue::flush() {
	blocked = true;

	//signal(condMutex);
}
*/
