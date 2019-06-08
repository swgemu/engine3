/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "MessageQueue.h"

MessageQueue::MessageQueue() : Vector<Message*>(1000, 1000), Condition(), Logger("MessageQueue") {
	blocked = false;
	//waitingForMessage = false;

	condMutex = new Mutex("MessageQueue");

	setLogging(false);
	setGlobalLogging(true);
	//condMutex->setMutexLogging(false);
}

MessageQueue::~MessageQueue() {
	delete condMutex;
}

void MessageQueue::push(Message* msg) {
	condMutex->lock();

	if (blocked) {
		condMutex->unlock();
		return;
	}

	Vector<Message*>::add(msg);

	#ifdef TRACE_MESSAGES
		StringBuffer s;
		s << size() << " messages in queue";
		info(s);
	#endif

	//if (waitingForMessage)
	signal(condMutex);

	condMutex->unlock();
}

/*void MessageQueue::printInfo() {
	StringBuffer msg;
	msg << "currently " << size() << " messages in queue";
	info(msg.toString(), true);
}*/

Message* MessageQueue::pop() {
	condMutex->lock();

	#ifdef TRACE_MESSAGES
		info("waiting message");
	#endif

	while (isEmpty()) {
		if (blocked) {
			condMutex->unlock();
			return nullptr;
		}

		//waitingForMessage = true;
		wait(condMutex);
	}

	Message* msg = remove(0);

	#ifdef TRACE_MESSAGES
		StringBuffer s;
		s << size() << " messages remained in queue";
		info(s);
	#endif

	//waitingForMessage = false;
	condMutex->unlock();

	return msg;
}

void MessageQueue::flush() {
	condMutex->lock();

	blocked = true;

	//if (waitingForMessage)
		signal(condMutex);

	condMutex->unlock();
}
