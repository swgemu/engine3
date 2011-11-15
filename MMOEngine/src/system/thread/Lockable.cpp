/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "system/lang/Time.h"
#include "system/lang/System.h"

#include "Thread.h"

#include "Lockable.h"

Lockable::Lockable() {
	threadLockHolder = NULL;

	lockCount = 0;
	currentCount = 0;

	trace = NULL;
	unlockTrace = NULL;

	//lockTime = new Time();

	doLog = true;
	doTrace = true;
}

Lockable::Lockable(const String& s) {
	threadLockHolder = NULL;

	//lockName = s;

	lockCount = 0;

	trace = NULL;
	unlockTrace = NULL;

	//lockTime = new Time();

	doLog = true;
	doTrace = true;
}

Lockable::~Lockable() {
	if (trace != NULL) {
		delete trace;
		trace = NULL;
	}

	if (unlockTrace != NULL) {
		delete unlockTrace;
		unlockTrace = NULL;
	}

	//delete lockTime;
}

void Lockable::traceDeadlock(const char* modifier) {
#ifdef TRACE_LOCKS
	System::out << "[" << Thread::getCurrentThread()->getName() << "] (" << Time::currentNanoTime() << " nsec) WARNING" << "[" << lockName << "] unable to access "
				<< modifier << "lock #" << currentCount << " at\n";
#endif

	StackTrace::printStackTrace();

	if (trace != NULL) {
		System::out << "[" << threadLockHolder->getName() << "] locked at " << lockTime.getMiliTime() << " by\n";
		trace->print();
	} else {
		System::out << "no previous stackTrace created\n";
	}

	while (true) {
		Thread::sleep(100);
	}
}
