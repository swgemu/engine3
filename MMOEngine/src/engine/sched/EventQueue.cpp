/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include <errno.h>

#include "EventQueue.h"

EventQueue::EventQueue() : AdvancedBinaryHeap(), Condition(), Logger("EventQueue") {
	condMutex = new Mutex("EventQueue");

	blocked = false;

	waitingForEvent = false;
	changePlan = false;

	setLogging(false);
	condMutex->setMutexLogging(false);
}

EventQueue::~EventQueue() {
	delete condMutex;
}

void EventQueue::add(Event* e, uint64 time) {
	condMutex->lock();

	if (e->isQueued())
		remove(e, false);

	if (time != 0)
		e->update(time);

	add(e, false);

	condMutex->unlock();
}

void EventQueue::add(Event* e, Time& time) {
	condMutex->lock();

	if (e->isQueued())
		remove(e, false);

	e->set(time);
	add(e, false);

	condMutex->unlock();
}

void EventQueue::add(Event* e, bool doLock) {
	condMutex->lock(doLock);

	if (blocked) {
		condMutex->unlock(doLock);
		return;
	} /*else if (e->getTimeStamp().isPast()) {
		error("past event scheduled - " + e->toString());

		StackTrace trace;
		trace.print();

		raise(SIGSEGV);
	}*/

	if (e->isQueued())
		remove(e, false);

	e->setQueued(true);

	#ifdef TRACE_EVENTS
		stringstream s;
		s << "added event " << e->getTimeStamp().getMiliTime() << "(" << e << ")";
		info(s);
	#endif

	AdvancedBinaryHeap::add(e);

	if (AdvancedBinaryHeap::getMin() == e && waitingForEvent) {
		changePlan = true;
		signal(condMutex);
	}

	condMutex->unlock(doLock);
}

Event* EventQueue::get() {
	condMutex->lock();

	waitingForEvent = true;

	while (true) {
		while (AdvancedBinaryHeap::isEmpty()) {
			if (blocked) {
				condMutex->unlock();
				return NULL;
			}

			#ifdef TRACE_EVENTS
				info("waiting event");
			#endif

			wait(condMutex);
		}

		Event* e = (Event*) AdvancedBinaryHeap::getMin();
		Time& time = e->getTimeStamp();

		if (blocked || !time.isFuture())
			break;

		#ifdef TRACE_EVENTS
			stringstream s;
			s << "scheduling " << e->toString();
			info(s);
		#endif

		int res = timedWait(condMutex, &time);

		#ifdef TRACE_EVENTS
			stringstream s2;
			s2 << "timedwait finished with (" << res << ")";
			info(s2);
		#endif

		if (res != 0 && res != ETIMEDOUT) {
			stringstream msg;

			if (res == 22)
				msg << "invalid condition parameters";
			else if (time.isFuture())
				msg << "misstiming (" << res << ") - future event happened" << e->getTimeStamp().getMiliTime();
			else
				msg << "condition error (" << res << ")";

			error(msg);
			continue;
		}

		if (changePlan) {
			#ifdef TRACE_EVENTS
				info("changing plan");
			#endif

			changePlan = false;
		} else
			break;
	}

	Event* e = (Event*) AdvancedBinaryHeap::removeMin();
	e->setQueued(false);

	if (!blocked && e->getTimeStamp().isFuture()) {
		int64 difference = -(uint64) e->getTimeStamp().miliDifference();

		if (difference > 10) {
			stringstream msg;
			msg << "future event happenend " << e->getTimeStamp().getMiliTime() << " ("
				<< difference << ")";
			error(msg);
		}
	} /*if (!blocked && e->getTimeStamp().miliDifference() > 250) {
		stringstream msg;
		msg << "WARNING too high delay " << e->getTimeStamp().getMiliTime() << " ("
			<< e->getTimeStamp().miliDifference() << ")";
		error(msg);
	}*/

	#ifdef TRACE_EVENTS
		stringstream s;
		s << "got event " << e->getTimeStamp().getMiliTime() << " [" << size() << "]";
		info(s);
	#endif

	waitingForEvent = false;
	condMutex->unlock();

	return e;
}

bool EventQueue::remove(Event* e, bool doLock) {
	condMutex->lock(doLock);

	if (!e->isQueued()) {
		condMutex->unlock(doLock);
		return false;
	}

	Event* next = (Event*) AdvancedBinaryHeap::getMin();

	AdvancedBinaryHeap::remove(e);
	e->setQueued(false);

	#ifdef TRACE_EVENTS
		stringstream s;
		s << "removed event " << e->getTimeStamp().getMiliTime() << " [" << size() << "]";
		info(s);
	#endif

	if (waitingForEvent && next == e) {
		changePlan = true;
		signal(condMutex);
	}

	condMutex->unlock(doLock);

	return true;
}

void EventQueue::flush() {
	condMutex->lock();

	blocked = true;

	if (waitingForEvent)
		broadcast(condMutex);

	condMutex->unlock();
}

void EventQueue::repair() {
	condMutex->lock();

	info("fixing queue");

	if (waitingForEvent)
		signal(condMutex);

	condMutex->unlock();
}

void EventQueue::printQueue() {
	/*stringstream s;
	s << dec << "EventQueue list [" << size() << "] = ";

	for (int i = 0; i < size(); ++i) {
		Event* ev = SortedVector<Event*>::get(i);
		s << ev->getTimeStamp().getMiliTime() << ", " ;
	}

	info(s);*/
}
