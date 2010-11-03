/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/stm/TransactionalMemoryManager.h"

#include "engine/db/ObjectDatabaseManager.h"

#include "TaskScheduler.h"

#include "TaskManager.h"

#ifdef VERSION_PUBLIC
#include "../service/proto/events/BaseClientCleanUpEvent.hpp"
#endif

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
	Reference<Task*> task = NULL;

	while ((task = tasks.get()) != NULL) {
		blockMutex.rlock();

		try {
		#ifdef VERSION_PUBLIC
			DO_TIMELIMIT;
		#endif

			task->execute();
		} catch (Exception& e) {
			error(e.getMessage());
			e.printStackTrace();
		} catch (...) {
			#ifdef VERSION_PUBLIC
			ObjectDatabaseManager::instance()->commitLocalTransaction();
				return;
			#else
				error("[TaskScheduler] unreported Exception caught");
			#endif
		}

		task->release();

		blockMutex.runlock();
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

bool TaskScheduler::scheduleTask(Task* task, uint64 delay) {
	/*if  (!task->setTaskScheduler(this))
		return false;*/

	return tasks.add(task, delay);
}

bool TaskScheduler::scheduleTask(Task* task, Time& time) {
	/*if  (!task->setTaskScheduler(this))
		return false;*/

	return tasks.add(task, time);
}

bool TaskScheduler::cancelTask(Task* task) {
	/*if (!task->clearTaskScheduler())
		return false;*/

	return tasks.remove(task);
}

void TaskScheduler::addSchedulerTasks(TaskScheduler* scheduler) {
	tasks.merge(scheduler->tasks);
}
