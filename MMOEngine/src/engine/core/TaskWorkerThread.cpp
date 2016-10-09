/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/stm/TransactionalMemoryManager.h"

#include "TaskWorkerThread.h"

TaskWorkerThread::TaskWorkerThread(const String& s, TaskQueue* queue, int cpu) : ServiceThread(s) {
	setInfoLogLevel();
	setGlobalLogging(true);

	this->cpu = cpu;
	this->queue = queue;
}

TaskWorkerThread::~TaskWorkerThread() {
}

void TaskWorkerThread::start(TaskManager* manager) {
	taskManager = manager;

	debug("started");

	ServiceThread::start(false);
}

void TaskWorkerThread::run() {
	if (cpu) {
		assignToCPU(cpu);
	}

	Task* task = NULL;

	while ((task = queue->pop()) != NULL) {
		blockMutex.lock();

		try {
//			debug("executing task");

			task->doExecute();
		} catch (Exception& e) {
			error(e.getMessage());
		} catch (...) {
			error("unreported Exception caught");
		}

#ifdef COLLECT_TASKSTATISTICS
		uint64 elapsedTime = task->getLastElapsedTime();
#endif

		blockMutex.unlock();

#ifdef COLLECT_TASKSTATISTICS
		const char* taskName = task->getTaskName();

		Locker guard(&tasksStatsGuard);

		Entry<const char*, TaskStatistics>* entry = tasksStatistics.getEntry(taskName);

		TaskStatistics* stats = NULL;

		if (entry == NULL) {
			TaskStatistics stats;

			stats.totalRunCount = 1;
			stats.totalRunTime = elapsedTime;
			stats.maxRunTime = elapsedTime;
			stats.minRunTime = elapsedTime;

			tasksStatistics.put(taskName, stats);
		} else {
			TaskStatistics& stats = entry->getValue();

			stats.totalRunTime += elapsedTime;

			if (stats.maxRunTime < elapsedTime) {
				stats.maxRunTime = elapsedTime;
			}

			if (stats.minRunTime > elapsedTime) {
				stats.minRunTime = elapsedTime;
			}

			++stats.totalRunCount;
		}
#endif

		task->release();

		task = NULL;
	}
}

#ifdef COLLECT_TASKSTATISTICS
HashTable<const char*, TaskStatistics> TaskWorkerThread::getTasksStatistics() {
	HashTable<const char*, TaskStatistics> ret;

	ReadLocker locker(&tasksStatsGuard);

	ret = tasksStatistics;

	return ret;
}

void TaskWorkerThread::clearTaskStatistics() {
	Locker locker(&tasksStatsGuard);

	tasksStatistics.removeAll();
}
#endif

void TaskWorkerThread::stop() {
	ServiceThread::stop();

	info("stopped");
}
