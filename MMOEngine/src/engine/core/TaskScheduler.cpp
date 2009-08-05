/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "TaskScheduler.h"

#ifdef VERSION_PUBLIC
#include "../service/proto/events/BaseClientCleanUpEvent.hpp"
#endif

TaskScheduler::TaskScheduler() : Thread(), Logger("TaskScheduler") {
	doRun = false;

	tasks.setTaskScheduler(this);

	tasks.setLoggingName("TaskQueue");
	tasks.setMutexName("TaskQueueLock");
}

TaskScheduler::TaskScheduler(const String& s) : Thread(), Logger(s) {
	doRun = false;

	tasks.setLoggingName(s + "TaskQueue");
	tasks.setMutexName(s + "TaskQueueLock");

	setLogging(false);
}

TaskScheduler::~TaskScheduler() {
}

void TaskScheduler::start() {
	StringBuffer msg;
	msg << "started";
	info(msg);

	doRun = true;
	Thread::start();
}

void TaskScheduler::run() {
	Task* task;

	while ((task = tasks.get()) != NULL) {
		try {
			#ifdef VERSION_PUBLIC
				DO_TIMELIMIT;
			#endif

			task->run();

			if (!task->isReentrant())
				delete task;
		} catch (Exception& e) {
			error(e.getMessage());
		} catch (...) {
			#ifdef VERSION_PUBLIC
				return;
			#else
				error("[TaskScheduler] unreported Exception caught");
			#endif
		}
	}
}

void TaskScheduler::stop() {
	if (doRun) {
		doRun = false;
		tasks.flush();

		join();
	}

	info("stopped");
}
