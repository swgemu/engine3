/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/stm/TransactionalMemoryManager.h"

#include "TaskWorkerThread.h"

TaskWorkerThread::TaskWorkerThread(const String& s, TaskQueue* queue, int cpu, bool blockDuringSave) : ServiceThread(s) {
	setInfoLogLevel();
	setGlobalLogging(true);

	this->cpu = cpu;
	this->queue = queue;
	this->blockDuringSave = blockDuringSave;
}

TaskWorkerThread::~TaskWorkerThread() {
}

void TaskWorkerThread::start() {
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

		Entry<const char*, RunStatistics>* entry = tasksStatistics.getEntry(taskName);

		RunStatistics* stats = NULL;

		if (entry == NULL) {
			RunStatistics stats;

			stats.totalRunCount = 1;
			stats.totalRunTime = elapsedTime;
			stats.maxRunTime = elapsedTime;
			stats.minRunTime = elapsedTime;

			tasksStatistics.put(taskName, stats);
		} else {
			RunStatistics& stats = entry->getValue();

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
HashTable<const char*, RunStatistics> TaskWorkerThread::getTasksStatistics() {
	ReadLocker locker(&tasksStatsGuard);

	HashTable<const char*, RunStatistics> ret = tasksStatistics;

	return ret;
}

VectorMap<String, RunStatistics> TaskWorkerThread::getLuaTasksStatistics() {
	ReadLocker locker(&tasksStatsGuard);

	VectorMap<String, RunStatistics> ret = luaTasksStatistics;

	return ret;
}

void TaskWorkerThread::clearTaskStatistics() {
	Locker locker(&tasksStatsGuard);

	tasksStatistics.removeAll();
	luaTasksStatistics.removeAll();
}

void TaskWorkerThread::addLuaTaskStats(const String& taskName, uint64 elapsedTime) {
	Locker guard(&tasksStatsGuard);

	auto entry = luaTasksStatistics.find(taskName);

	RunStatistics* stats = NULL;

	if (entry == -1) {
		RunStatistics stats;

		stats.totalRunCount = 1;
		stats.totalRunTime = elapsedTime;
		stats.maxRunTime = elapsedTime;
		stats.minRunTime = elapsedTime;

		luaTasksStatistics.put(taskName, stats);
	} else {
		RunStatistics& stats = luaTasksStatistics.elementAt(entry).getValue();

		stats.totalRunTime += elapsedTime;

		if (stats.maxRunTime < elapsedTime) {
			stats.maxRunTime = elapsedTime;
		}

		if (stats.minRunTime > elapsedTime) {
			stats.minRunTime = elapsedTime;
		}

		++stats.totalRunCount;
	}
}

#ifdef CXX11_COMPILER
void TaskWorkerThread::addLuaTaskStats(String&& taskName, uint64 elapsedTime) {
	Locker guard(&tasksStatsGuard);

	auto entry = luaTasksStatistics.find(taskName);

	RunStatistics* stats = NULL;

	if (entry == -1) {
		RunStatistics stats;

		stats.totalRunCount = 1;
		stats.totalRunTime = elapsedTime;
		stats.maxRunTime = elapsedTime;
		stats.minRunTime = elapsedTime;

		luaTasksStatistics.put(std::move(taskName), std::move(stats));
	} else {
		RunStatistics& stats = luaTasksStatistics.elementAt(entry).getValue();

		stats.totalRunTime += elapsedTime;

		if (stats.maxRunTime < elapsedTime) {
			stats.maxRunTime = elapsedTime;
		}

		if (stats.minRunTime > elapsedTime) {
			stats.minRunTime = elapsedTime;
		}

		++stats.totalRunCount;
	}
}
#endif
#endif

void TaskWorkerThread::stop() {
	ServiceThread::stop();

	info("stopped");
}
