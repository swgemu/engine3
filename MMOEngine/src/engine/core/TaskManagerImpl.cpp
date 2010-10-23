/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/db/ObjectDatabaseManager.h"

#include "TaskWorkerThread.h"

#include "TaskManagerImpl.h"

#include "TaskScheduler.h"

#include "Task.h"
#include "ReentrantTask.h"

TaskManagerImpl::TaskManagerImpl() : Mutex("TaskManager"), Logger("TaskManager") {
	currentTaskScheduler = 0;

	shuttingDown = false;
}

TaskManagerImpl::~TaskManagerImpl() {
}

void TaskManagerImpl::initialize() {
	initialize(DEAFULT_WORKER_THREADS, DEAFULT_SCHEDULER_THREADS);
}

void TaskManagerImpl::initialize(int workerCount, int schedulerCount) {
	ObjectDatabaseManager::instance();

	Locker locker(this);

	for (int i = 0; i < workerCount; ++i) {
		TaskWorkerThread* worker = new TaskWorkerThread("TaskWorkerThread" + String::valueOf(i));

		workers.add(worker);
	}

	for (int i = 0; i < schedulerCount; ++i) {
		TaskScheduler* scheduler = new TaskScheduler("TaskScheduler" + String::valueOf(i));
		scheduler->setTaskManager(this);

		schedulers.add(scheduler);
	}

	StringBuffer msg;
	msg << "initialized";
	info(msg, true);
}

void TaskManagerImpl::start() {
	Locker locker(this);

	for (int i = 0; i < workers.size(); ++i) {
		TaskWorkerThread* worker = workers.get(i);
		worker->start(this);
	}

	for (int i = 0; i < schedulers.size(); ++i) {
		TaskScheduler* scheduler = schedulers.get(i);
		scheduler->start();
	}

	StringBuffer msg;
	msg << "started";
	info(msg, true);
}

void TaskManagerImpl::shutdown() {
	shuttingDown = true;

	while (!schedulers.isEmpty()) {
		lock();

		TaskScheduler* scheduler = schedulers.remove(0);

		unlock();

		scheduler->stop();

		delete scheduler;
	}

	tasks.flush();

	while (!workers.isEmpty()) {
		lock();

		TaskWorkerThread* worker = workers.remove(0);

		unlock();

		worker->stop();

		delete worker;
	}

	info("stopped");
}

TaskScheduler* TaskManagerImpl::getTaskScheduler() {
	Locker locker(this);

	if (schedulers.isEmpty()) {
		if (!shuttingDown)
			throw Exception("No schedulers available");
		else
			return NULL;
	}

	int index = currentTaskScheduler++ % schedulers.size();

	return schedulers.get(index);
}

void TaskManagerImpl::setTaskScheduler(Task* task, TaskScheduler* scheduler) {
	if (!task->setTaskScheduler(scheduler))
		throw IllegalArgumentException("task is already scheduled");
}

void TaskManagerImpl::executeTask(Task* task) {
	tasks.push(task);
}

void TaskManagerImpl::scheduleTask(Task* task, uint64 delay) {
	Locker locker(this);

	if (task->isScheduled())
		throw IllegalArgumentException("task is already scheduled");

	TaskScheduler* scheduler = getTaskScheduler();
	if (scheduler == NULL)
		return;

	locker.release();

	if (!scheduler->scheduleTask(task, delay))
		throw IllegalArgumentException("Task was invalid for scheduling");
}

void TaskManagerImpl::scheduleTask(Task* task, Time& time) {
	Locker locker(this);

	if (task->isScheduled())
		throw IllegalArgumentException("task is already scheduled");

	TaskScheduler* scheduler = getTaskScheduler();
	if (scheduler == NULL)
		return;

	locker.release();

	if (!scheduler->scheduleTask(task, time))
		throw IllegalArgumentException("Task was invalid for scheduling");
}

void TaskManagerImpl::rescheduleTask(Task* task, uint64 delay) {
	cancelTask(task);

	scheduleTask(task, delay);
}

void TaskManagerImpl::rescheduleTask(Task* task, Time& time) {
	cancelTask(task);

	scheduleTask(task, time);
}

bool TaskManagerImpl::cancelTask(Task* task) {
	Locker locker(this);

	if (!task->isScheduled())
		return false;

	if (isRunning()) {
		TaskScheduler* scheduler = task->getTaskScheduler();

		locker.release();

		return scheduler->cancelTask(task);
	} else {
		cancelledTasks.put(task);

		return true;
	}
}

void TaskManagerImpl::clearTasks() {
	Locker locker(this);

	tasks.removeAll();

	for (int i = 0; i < schedulers.size(); ++i) {
		TaskScheduler* scheduler = schedulers.get(i);

		scheduler->clearTasks();
	}

	cancelledTasks.removeAll();
}

void TaskManagerImpl::flushTasks() {
	Locker locker(this);

	tasks.flush();

	for (int i = 0; i < schedulers.size(); ++i) {
		TaskScheduler* scheduler = schedulers.get(i);

		scheduler->flushTasks();
	}

}

void TaskManagerImpl::mergeTasks(TaskManagerImpl* manager) {
	Locker locker(this);

	tasks.addAll(manager->tasks);

	for (int i = 0; i < cancelledTasks.size(); ++i) {
		Task* task = cancelledTasks.get(i);

		cancelTask(task);
	}

	TaskScheduler* scheduler = getTaskScheduler();

	// FIXME - temp hack
	scheduler->addSchedulerTasks(manager->getTaskScheduler());
}

void TaskManagerImpl::printInfo() {
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
				//TaskManagerImpl::instance()->scheduleTask(this, 100);

			for (int i = 0; i < 0x10000000; ++i) ;

		} catch (Exception& e) {
			e.printStackTrace();
		}
	}
};

void TaskManagerImpl::testScheduler() {
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

int TaskManagerImpl::getScheduledTaskSize() {
	int sum = 0;

	for (int i = 0; i < schedulers.size(); ++i) {
		TaskScheduler* scheduler = schedulers.get(i);

		sum += scheduler->getQueueSize();
	}

	return sum;
}

int TaskManagerImpl::getExecutingTaskSize() {
	return tasks.size();
}
