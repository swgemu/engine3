/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/stm/TransactionalMemoryManager.h"

#ifndef VERSION_PUBLIC
#define COUNT_SCHEDULER_TASKS
#endif

static Time startTime;

TaskScheduler::TaskScheduler() : Thread(), Logger("TaskScheduler") {
	taskManager = NULL;

	doRun = false;

	tasks.setTaskScheduler(this);

	tasks.setLoggingName("TaskQueue");
	tasks.setMutexName("TaskQueueLock");

	startTime.updateToCurrentTime();

	setInfoLogLevel();
	setGlobalLogging(true);
}

TaskScheduler::TaskScheduler(const String& s) : Thread(), Logger(s) {
	taskManager = NULL;

	doRun = false;

	startTime.updateToCurrentTime();

	tasks.setTaskScheduler(this);

	tasks.setLoggingName(s + "TaskQueue");
	tasks.setMutexName(s + "TaskQueueLock");

	setInfoLogLevel();
	setGlobalLogging(true);
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

void TaskScheduler::prepareTask(Task*) {

}

void TaskScheduler::run() {
	Reference<Task*> task = NULL;

	while (doRun && ((task = tasks.get()) != NULL)) {
		blockMutex.lock();

		try {
			task->doExecute();

		} catch (Exception& e) {
			error(e.getMessage());
			e.printStackTrace();
		} catch (...) {
			error("[TaskScheduler] unreported Exception caught");
		}

		task->release();

		blockMutex.unlock();

		if (task->isPeriodic()) {
			taskManager->scheduleTask(task, task->getPeriod());

			//assert(task->isScheduled());
		}

#ifdef COUNT_SCHEDULER_TASKS
		const char* taskName = task->getTaskName();

		Locker guard(&tasksCountGuard);

		Entry<const char*, uint64>* entry = tasksCount.getEntry(taskName);

		if (entry == NULL) {
			tasksCount.put(taskName, 1);
		} else {
			++(entry->getValue());
		}
#endif
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

	task->setPeriod(0);

	return tasks.remove(task);
}

void TaskScheduler::addSchedulerTasks(TaskScheduler* scheduler) {
	Task* task = NULL;

	while ((task = (Task*) scheduler->tasks.poll()) != NULL)
			tasks.add(task);

	//tasks.addAll(scheduler->tasks);
}

HashTable<const char*, uint64> TaskScheduler::getTasksCount() {
	ReadLocker guard(&tasksCountGuard);

	HashTable<const char*, uint64> copy = tasksCount;

	return copy;
}
