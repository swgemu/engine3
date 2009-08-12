/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "TaskWorkerThread.h"

#include "TaskManager.h"

#include "TaskScheduler.h"

#include "Task.h"
#include "ReentrantTask.h"

TaskManager::TaskManager() : Mutex("TaskManager"), Logger("TaskManager") {
	currentTaskScheduler = 0;
}

TaskManager::~TaskManager() {
}

void TaskManager::initialize() {
	lock();

	for (int i = 0; i < DEAFULT_WORKER_THREADS; ++i) {
		TaskWorkerThread* worker = new TaskWorkerThread("TaskWorkerThread" + String::valueOf(i));
		worker->start(this);

		workers.add(worker);
	}

	for (int i = 0; i < DEAFULT_SCHEDULER_THREADS; ++i) {
		TaskScheduler* scheduler = new TaskScheduler("TaskScheduler" + String::valueOf(i));
		scheduler->start();

		schedulers.add(scheduler);
	}

	StringBuffer msg;
	msg << "started";
	info(msg, true);

	unlock();
}

void TaskManager::shutdown() {
	lock();

	while (!schedulers.isEmpty()) {
		TaskScheduler* scheduler = schedulers.remove(0);
		scheduler->stop();

		delete scheduler;
	}

	while (!workers.isEmpty()) {
		TaskWorkerThread* worker = workers.remove(0);
		worker->stop();

		delete worker;
	}

	info("stopped");

	unlock();
}

TaskScheduler* TaskManager::getTaskScheduler(bool doLock) {
	lock(doLock);

	int index = currentTaskScheduler++ % schedulers.size();

	TaskScheduler* scheduler = schedulers.get(index);

	unlock(doLock);

	return scheduler;
}

void TaskManager::executeTask(Task* task) {
	tasks.push(task);
}

void TaskManager::scheduleTask(Task* task, uint64 delay) {
	TaskScheduler* scheduler = NULL;

	try {
		lock();

		if (task->isScheduled()) {
			unlock();

			return ;
		}

		scheduler = getTaskScheduler(false);

		task->setTaskScheduler(scheduler);

		unlock();
	} catch (...) {
		error("unreported exception caught on TaskManager::scheduleTask()");

		unlock();
	}

	scheduler->scheduleTask(task, delay);
}

void TaskManager::scheduleTask(Task* task, Time& time) {
	TaskScheduler* scheduler = NULL;

	try {
		lock();

		if (task->isScheduled()) {
			unlock();

			return ;
		}

		scheduler = getTaskScheduler(false);

		task->setTaskScheduler(scheduler);

		unlock();
	} catch (...) {
		error("unreported exception caught on TaskManager::scheduleTask()");

		unlock();
	}

	scheduler->scheduleTask(task, time);
}

void TaskManager::rescheduleTask(Task* task, uint64 delay) {
	TaskScheduler* scheduler = NULL;

	try {
		lock();

		if (task->isScheduled())
			scheduler = task->getTaskScheduler();
		else
			scheduler = getTaskScheduler(false);

		unlock();
	} catch (...) {
		error("unreported exception caught on TaskManager::rescheduleTask()");

		unlock();
	}

	scheduler->rescheduleTask(task, delay);
}

void TaskManager::rescheduleTask(Task* task, Time& time) {
	TaskScheduler* scheduler = NULL;

	try {
		lock();

		if (task->isScheduled())
			scheduler = task->getTaskScheduler();
		else
			scheduler = getTaskScheduler(false);

		unlock();
	} catch (...) {
		error("unreported exception caught on TaskManager::rescheduleTask()");

		unlock();
	}

	scheduler->rescheduleTask(task, time);
}

bool TaskManager::cancelTask(Task* task) {
	try {
		lock();

		if (!task->isScheduled()) {
			unlock();

			return false;
		}

		unlock();
	} catch (...) {
		error("unreported exception caught on TaskManager::rescheduleTask()");

		unlock();
	}

	TaskScheduler* scheduler = task->getTaskScheduler();
	scheduler->cancelTask(task);

	return true;
}

void TaskManager::printInfo() {
	lock();

	StringBuffer msg;
	msg << "executing tasks - " << getExecutingTaskSize();
	info(msg, true);

	StringBuffer msg2;
	msg2 << "scheduled tasks - " << getScheduledTaskSize();
	info(msg2, true);

	unlock();
}

class TestTask : public Task {
public:
	void run() {
		System::out.print("test");
	}
};

void TaskManager::testScheduler() {
	for (int i = 0; i < 100; ++i) {
		uint32 shift = System::random(250);

		scheduleTask(new TestTask(), 1000 + shift);
	}
}

int TaskManager::getScheduledTaskSize() {
	int sum = 0;

	for (int i = 0; i < schedulers.size(); ++i) {
		TaskScheduler* scheduler = schedulers.get(i);

		sum += scheduler->getQueueSize();
	}

	return sum;
}

int TaskManager::getExecutingTaskSize() {
	return tasks.size();
}
