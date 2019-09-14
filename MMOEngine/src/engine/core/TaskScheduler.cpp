/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/stm/TransactionalMemoryManager.h"

#ifndef VERSION_PUBLIC
//#define COUNT_SCHEDULER_TASKS
#endif

static Time startTime;

TaskScheduler::TaskScheduler() : Thread(), Logger("TaskScheduler") {
	taskManager = nullptr;

	doRun = false;

	pause = false;

	tasks.setTaskScheduler(this);

	tasks.setLoggingName("TaskQueue");
	tasks.setMutexName("TaskQueueLock");

	startTime.updateToCurrentTime();

	setInfoLogLevel();
	setGlobalLogging(true);
}

TaskScheduler::TaskScheduler(const String& s) : Thread(), Logger(s) {
	taskManager = nullptr;

	doRun = false;

	pause = false;

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
	info() << "started";

	doRun = true;
	Thread::start();
}

void TaskScheduler::prepareTask(Task*) {

}

void TaskScheduler::setPause(bool val) {
	pause = val;

	if (val) {
		tasks.wake();
	}
}

void TaskScheduler::run() {
	Task* task = nullptr;

	while (doRun) {
		auto task = tasks.get();

		blockMutex.lock();

		if (task == nullptr) {
			blockMutex.unlock();

			do {
				Thread::sleep(0, 1);
				Thread::yield();
			} while (pause);

			continue;
		}

		try {
			if (!task->doRunInScheduler()) {
				task->execute();
			} else {
				task->executeInThread();
			}

		} catch (Exception& e) {
			error(e.getMessage());
			e.printStackTrace();
		} catch (...) {
			error("[TaskScheduler] unreported Exception caught");
		}

		blockMutex.unlock();

		try {
			if (task->isPeriodic()) {
				taskManager->scheduleTask(task, task->getPeriod());

				//assert(task->isScheduled());
			}

#ifdef COUNT_SCHEDULER_TASKS
			const char* taskName = task->getTaskName();

			Locker guard(&tasksCountGuard);

			Entry<const char*, uint64>* entry = tasksCount.getEntry(taskName);

			if (entry == nullptr) {
				tasksCount.put(taskName, 1);
			} else {
				++(entry->getValue());
			}
#endif
		} catch (...) {
			error("[TaskScheduler] unreported Exception caught");
		}

		task->release();

		while (pause) {
			Thread::sleep(1);
			Thread::yield();

			continue;
		}
	}
}

void TaskScheduler::stop(){
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

bool TaskScheduler::scheduleTask(Task* task, const Time& time) {
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
	Task* task = nullptr;

	while ((task = (Task*) scheduler->tasks.poll()) != nullptr)
			tasks.add(task);

	//tasks.addAll(scheduler->tasks);
}

HashTable<String, uint64> TaskScheduler::getTasksCount() {
	ReadLocker guard(&tasksCountGuard);

	auto copy = tasksCount;

	return copy;
}
