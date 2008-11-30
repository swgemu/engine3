/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "MessageQueue.h"

MessageQueue::MessageQueue() : SortedVector<Message*>(100, 50), Condition(), Logger("MessageQueue") {
	blocked = false;
	waitingForMessage = false;
		
	condMutex = new Mutex("MessageQueue");

	setLogging(false);
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
		
	SortedVector<Message*>::put(msg);

	#ifdef TRACE_MESSAGES		
		StringBuffer s;
		s << size() << " messages in queue";
		info(s);
	#endif
		
	if (waitingForMessage)
		signal(condMutex);
		
	condMutex->unlock();
}
	
Message* MessageQueue::pop() {
	condMutex->lock();

	#ifdef TRACE_MESSAGES		
		info("waiting message");
	#endif
		
	while (isEmpty()) {
		if (blocked) {
			condMutex->unlock();
			return NULL;
		}

		waitingForMessage = true;
		wait(condMutex);
	}

	Message* msg = remove(0);

	#ifdef TRACE_MESSAGES		
		StringBuffer s;
		s << size() << " messages remained in queue";
		info(s);
	#endif
		
	waitingForMessage = false;
	condMutex->unlock();
		
	return msg;
}
	
void MessageQueue::flush() {
	condMutex->lock();

	blocked = true;
		
	if (waitingForMessage)
		signal(condMutex);
		
	condMutex->unlock();
}
