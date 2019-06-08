/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "system/lang/Time.h"

#include "Thread.h"

#include "Lockable.h"

Lockable::Lockable() {
	threadLockHolder = nullptr;

#ifdef TRACE_LOCKS
	trace = nullptr;
	unlockTrace = nullptr;

	doTrace = true;
#endif

	//lockTime = new Time();

#ifdef LOG_LOCKS
	currentCount = 0;

	lockCount = 0;

	doLog = true;
#endif
}

Lockable::Lockable(const String& s) {
	threadLockHolder = nullptr;

	//lockName = s;

#ifdef TRACE_LOCKS
	trace = nullptr;
	unlockTrace = nullptr;

	doTrace = true;
#endif

	//lockTime = new Time();

#ifdef LOG_LOCKS
	currentCount = 0;

	lockCount = 0;

	doLog = true;
#endif
}

Lockable::~Lockable() {
#ifdef TRACE_LOCKS
	if (trace != nullptr) {
		delete trace;
		trace = nullptr;
	}

	if (unlockTrace != nullptr) {
		delete unlockTrace;
		unlockTrace = nullptr;
	}
#endif

	//delete lockTime;
}

void Lockable::traceDeadlock(const char* modifier) {
#ifdef TRACE_LOCKS
	System::out << "[" << Thread::getCurrentThread()->getName() << "] (" << Time::currentNanoTime() << " nsec) WARNING" << "[" << lockName << "] unable to access "
				<< modifier << "lock #" << currentCount << " at\n";
#endif

	StackTrace::printStackTrace();

#ifdef TRACE_LOCKS
	if (trace != nullptr) {
		System::out << "[" << threadLockHolder->getName() << "] locked at " << lockTime.getMiliTime() << " by\n";
		trace->print();
	} else {
		System::out << "no previous stackTrace created\n";
	}
#endif

	while (true) {
		Thread::sleep(100);
	}
}
