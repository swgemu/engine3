/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ScheduleManager.h"

#ifdef VERSION_PUBLIC
#include "../service/proto/events/BaseClientCleanUpEvent.hpp"
#endif

ScheduleManager::ScheduleManager() : Thread(), Mutex("Scheduler"), Logger("Scheduler") {
	doRun = false;
		
	string logname = "SchedulerEventQueue";
	events.setLoggingName(logname);
		
	string mutexname = "SchedulerEventQueueLock";
	events.setMutexName(mutexname);
}

ScheduleManager::ScheduleManager(const string& s) : Thread(), Mutex("SchedManager"), Logger(s) {
	doRun = false;
		
	string logname = s + "EventQueue";
	events.setLoggingName(logname);
		
	string mutexname = s + "EventQueueLock";
	events.setMutexName(mutexname);
	
	setLogging(false);
}

ScheduleManager::~ScheduleManager() {
}

void ScheduleManager::start() {
	stringstream msg;
	msg << "started";
	info(msg); 

	doRun = true;
	Thread::start();
}

void ScheduleManager::run() {
	Event* event;

	while ((event = events.get()) != NULL) {
		try {
			#ifdef VERSION_PUBLIC
				DO_TIMELIMIT
			#endif
			
			event->activate();
		
			if (!event->doKeep())
				delete event;
		} catch (SocketException& e) {
			info(e.getMessage());
		} catch (Exception& e) {
			error(e.getMessage());
		} catch (...) {
			#ifdef VERSION_PUBLIC
				return;
			#else
				cout << "[ScheduleManager] unreported Exception caught\n";
			#endif
		}
	}
}

void ScheduleManager::stop() {
	if (doRun) {
		doRun = false;
		events.flush();
		
		join();
	}
	
	info("stopped");
}
