/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/stm/TransactionalMemoryManager.h"

#include "TaskScheduler.h"

#include "engine/db/ObjectDatabaseManager.h"

#ifdef VERSION_PUBLIC
#include "../service/proto/events/BaseClientCleanUpEvent.hpp"
#endif

#include "TaskManager.h"

TaskScheduler::TaskScheduler() : Thread(), Logger("TaskScheduler") {
	taskManager = NULL;

	doRun = false;

	tasks.setTaskScheduler(this);

	tasks.setLoggingName("TaskQueue");
	tasks.setMutexName("TaskQueueLock");
}

TaskScheduler::TaskScheduler(const String& s) : Thread(), Logger(s) {
	taskManager = NULL;

	doRun = false;

	tasks.setTaskScheduler(this);

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
	ObjectDatabaseManager::instance()->commitLocalTransaction();

	Reference<Task*> task = NULL;

	while ((task = tasks.get()) != NULL) {
		ObjectDatabaseManager::instance()->startLocalTransaction();

		try {
		#ifdef VERSION_PUBLIC
			DO_TIMELIMIT;
		#endif

			if (!task->setTaskScheduler(NULL))
				throw IllegalArgumentException("task was already unscheduled");

			task->execute();
		} catch (Exception& e) {
			error(e.getMessage());
		} catch (...) {
			#ifdef VERSION_PUBLIC
			ObjectDatabaseManager::instance()->commitLocalTransaction();
				return;
			#else
				error("[TaskScheduler] unreported Exception caught");
			#endif
		}

		ObjectDatabaseManager::instance()->commitLocalTransaction();

		task->release();
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

void TaskScheduler::addSchedulerTasks(TaskScheduler* scheduler) {
	tasks.merge(scheduler->tasks);
}
