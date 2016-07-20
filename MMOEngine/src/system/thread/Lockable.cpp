/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "system/lang/Time.h"

#include "Thread.h"

#include "Lockable.h"

Lockable::Lockable() {
	threadLockHolder = NULL;

#ifdef TRACE_LOCKS
	trace = NULL;
	unlockTrace = NULL;

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
	threadLockHolder = NULL;

	//lockName = s;

#ifdef TRACE_LOCKS
	trace = NULL;
	unlockTrace = NULL;

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
	if (trace != NULL) {
		delete trace;
		trace = NULL;
	}

	if (unlockTrace != NULL) {
		delete unlockTrace;
		unlockTrace = NULL;
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
	if (trace != NULL) {
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
