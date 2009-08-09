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
	info(msg);

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

TaskScheduler* TaskManager::getTaskScheduler() {
	lock();

	int index = currentTaskScheduler++ % schedulers.size();

	TaskScheduler* scheduler = schedulers.get(index);

	unlock();

	return scheduler;
}

void TaskManager::executeTask(Task* task) {
	tasks.push(task);
}

void TaskManager::scheduleTask(Task* task, uint64 delay) {
	TaskScheduler* scheduler = getTaskScheduler();

	scheduler->scheduleTask(task, delay);
}

void TaskManager::scheduleTask(Task* task, Time& time) {
	TaskScheduler* scheduler = getTaskScheduler();

	scheduler->scheduleTask(task, time);
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
	return 0;
}
