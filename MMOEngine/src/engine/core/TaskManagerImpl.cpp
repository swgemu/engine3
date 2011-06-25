/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/db/ObjectDatabaseManager.h"

#include "TaskWorkerThread.h"

#include "TaskManagerImpl.h"

#include "TaskScheduler.h"

#include "Task.h"

TaskManagerImpl::TaskManagerImpl() : Mutex("TaskManager"), Logger("TaskManager") {
	shuttingDown = false;

	setInfoLogLevel();
}

TaskManagerImpl::~TaskManagerImpl() {
}

void TaskManagerImpl::initialize() {
	initialize(DEFAULT_WORKER_THREADS, DEFAULT_SCHEDULER_THREADS);
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
	debug(msg);
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
	debug(msg);
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

	debug("stopped");
}

Vector<Locker*>* TaskManagerImpl::blockTaskManager() {
	//Locker locker(this);

	Vector<Locker*>* lockers = new Vector<Locker*>();

	for (int i = 0; i < workers.size(); ++i) {
		TaskWorkerThread* worker = workers.get(i);

		Mutex* blockMutex = worker->getBlockMutex();

		Locker* locker = new Locker(blockMutex);
		lockers->add(locker);
	}

	for (int i = 0; i < schedulers.size(); ++i) {
		TaskScheduler* scheduler = schedulers.get(i);

		Mutex* blockMutex = scheduler->getBlockMutex();

		Locker* locker = new Locker(blockMutex);
		lockers->add(locker);
	}

	return lockers;
}

void TaskManagerImpl::unblockTaskManager(Vector<Locker*>* lockers) {
	for (int i = 0; i < lockers->size(); ++i)
		delete lockers->get(i);

	lockers->removeAll();
}

TaskScheduler* TaskManagerImpl::getTaskScheduler() {
	Locker locker(this);

	if (schedulers.isEmpty()) {
		if (!shuttingDown)
			throw Exception("No schedulers available");
		else
			return NULL;
	}

	int index = currentTaskScheduler.increment() % schedulers.size();

	return schedulers.get(index);
}

void TaskManagerImpl::setTaskScheduler(Task* task, TaskScheduler* scheduler) {
	if (!task->setTaskScheduler(scheduler))
		throw IllegalArgumentException("task is already scheduled");
}

void TaskManagerImpl::executeTask(Task* task) {
	tasks.push(task);
}

void TaskManagerImpl::executeTaskFront(Task* task) {
	tasks.pushFront(task);
}

void TaskManagerImpl::executeTaskRandom(Task* task) {
	tasks.pushRandom(task);
}

void TaskManagerImpl::executeTasks(const Vector<Task*>& taskList) {
	tasks.pushAll(taskList);
}

bool TaskManagerImpl::getNextExecutionTime(Task* task, Time& nextExecutionTime) {
	nextExecutionTime = task->getNextExecutionTime();

	return true;
}

bool TaskManagerImpl::isTaskScheduled(Task* task) {
	return task->getTaskScheduler() != NULL;
}

void TaskManagerImpl::scheduleTask(Task* task, uint64 delay) {
	Locker locker(this);

	if (task->isScheduled()) {
	#ifdef TRACE_TASKS
		task->printScheduleTrace();
	#endif

		throw IllegalArgumentException("task is already scheduled");
	}

	TaskScheduler* scheduler = getTaskScheduler();
	if (scheduler == NULL)
		return;

	locker.release();

	if (!scheduler->scheduleTask(task, delay))
		throw IllegalArgumentException("Task was invalid for scheduling");
}

void TaskManagerImpl::scheduleTask(Task* task, Time& time) {
	Locker locker(this);

	if (task->isScheduled()) {
	#ifdef TRACE_TASKS
		task->printScheduleTrace();
	#endif

		throw IllegalArgumentException("task is already scheduled");
	}

	TaskScheduler* scheduler = getTaskScheduler();
	if (scheduler == NULL)
		return;

	locker.release();

	if (!scheduler->scheduleTask(task, time))
		throw IllegalArgumentException("Task was invalid for scheduling");
}

void TaskManagerImpl::rescheduleTask(Task* task, uint64 delay) {
	task->acquire();

	cancelTask(task);

	scheduleTask(task, delay);

	task->release();
}

void TaskManagerImpl::rescheduleTask(Task* task, Time& time) {
	task->acquire();

	cancelTask(task);

	scheduleTask(task, time);

	task->release();
}

bool TaskManagerImpl::cancelTask(Task* task) {
	Locker locker(this);

	if (!task->isScheduled())
		return false;

	TaskScheduler* scheduler = task->getTaskScheduler();
	//assert (scheduler != NULL);

	locker.release();

	if (scheduler == NULL)
		return false;

	return scheduler->cancelTask(task);
}

void TaskManagerImpl::flushTasks() {
	Locker locker(this);

	tasks.flush();

	for (int i = 0; i < schedulers.size(); ++i) {
		TaskScheduler* scheduler = schedulers.get(i);

		scheduler->flushTasks();
	}

}

void TaskManagerImpl::printInfo() {
	lock();

	StringBuffer msg;
	msg << "executing tasks - " << getExecutingTaskSize();
	info(msg);

	StringBuffer msg2;
	msg2 << "scheduled tasks - " << getScheduledTaskSize();
	info(msg2);

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

void TaskManagerImpl::testScheduler() {
	for (int i = 0; i < 20; ++i) {
		uint32 shift = System::random(100);

		Task* task = new TestTask(i);

		scheduleTask(task, 100 + shift);
		rescheduleTask(task, shift - 50);

		Thread::sleep(System::random(5) + 5);
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
