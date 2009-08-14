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

void TaskManager::setTaskScheduler(Task* task, TaskScheduler* scheduler, bool doLock) {
	lock(doLock);

	task->setTaskScheduler(scheduler);

	unlock(doLock);
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

			throw IllegalArgumentException("task is already scheduled");
		}

		scheduler = getTaskScheduler(false);

		setTaskScheduler(task, scheduler, false);

		unlock();
	} catch (...) {
		error("unreported exception caught on TaskManager::scheduleTask()");

		unlock();
	}

	if (!scheduler->scheduleTask(task, delay))
		throw IllegalArgumentException("Task was invalid for scheduling");
}

void TaskManager::scheduleTask(Task* task, Time& time) {
	TaskScheduler* scheduler = NULL;

	try {
		lock();

		if (task->isScheduled()) {
			unlock();

			throw IllegalArgumentException("task is already scheduled");
		}

		scheduler = getTaskScheduler(false);

		task->setTaskScheduler(scheduler);

		unlock();
	} catch (...) {
		error("unreported exception caught on TaskManager::scheduleTask()");

		unlock();
	}

	if (!scheduler->scheduleTask(task, time))
		throw IllegalArgumentException("Task was invalid for scheduling");
}

void TaskManager::rescheduleTask(Task* task, uint64 delay) {
	task->cancel();

	scheduleTask(task, delay);
}

void TaskManager::rescheduleTask(Task* task, Time& time) {
	task->cancel();

	scheduleTask(task, time);
}

bool TaskManager::cancelTask(Task* task) {
	TaskScheduler* scheduler = NULL;

	try {
		lock();

		if (!task->isScheduled()) {
			unlock();

			return false;
		}

		scheduler = task->getTaskScheduler();

		unlock();
	} catch (...) {
		error("unreported exception caught on TaskManager::rescheduleTask()");

		unlock();
		return false;
	}

	return scheduler->cancelTask(task);
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
	int value;

public:
	TestTask(int val) {
		value = val;
	}

	void run() {
		System::out.println("test" + String::valueOf(value));
	}
};

class ReentrantTestTask : public ReentrantTask {
	int value;

public:
	ReentrantTestTask(int val) {
		value = val;
	}

	void run() {
		try {
			//System::out.println("test" + String::valueOf(value));
			if (System::random(10) == 0)
				TaskManager::instance()->scheduleTask(this, 100);

			for (int i = 0; i < 0x10000000; ++i);
		} catch (Exception& e) {
			e.printStackTrace();
		}
	}
};

void TaskManager::testScheduler() {
	for (int i = 0; i < 20; ++i) {
		uint32 shift = System::random(100);

		Task* task = new TestTask(i);

		scheduleTask(task, 100 + shift);
		rescheduleTask(task, shift - 50);

		//Thread::sleep(System::random(5) + 5);
	}

	/*Task* task = new ReentrantTestTask(1);
	scheduleTask(task, 1000);*/
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
