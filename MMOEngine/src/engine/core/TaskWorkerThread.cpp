/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/stm/TransactionalMemoryManager.h"

#include "TaskWorkerThread.h"

#include "MetricsManager.h"

#define STATS_MAX_MYSQL_QUERIES 1000
#define STATS_STATSD_SAMPLE 0

TaskWorkerThread::TaskWorkerThread(const String& s, TaskQueue* queue, int cpu, bool blockDuringSave) : ServiceThread(s) {
	setInfoLogLevel();
	setGlobalLogging(true);

	this->cpu = cpu;
	this->queue = queue;
	this->blockDuringSave = blockDuringSave;

	pauseWorker = false;

#ifdef COLLECT_TASKSTATISTICS
	totalTaskRunCount = 0;

	samplingRate = STATS_STATSD_SAMPLE;
#endif
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

	while (doRun) {
		auto task = queue->pop();

		blockMutex.lock();

		if (task == nullptr) {
			blockMutex.unlock();

			do {
				Thread::sleep(1);
				Thread::yield();
			} while (pauseWorker);

			continue;
		}

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

		++totalTaskRunCount;

		if (samplingRate && ((totalTaskRunCount % samplingRate)) == 0) {
			char fullTaskName[256];
			snprintf(fullTaskName, 256, "engine3.tasks.%s", fullName);

			char metricsValue[48];
			snprintf(metricsValue, 48, "%g", (double) elapsedTime / 1000000);

			char sampleValue[48];
			snprintf(sampleValue, 48, "%g", 1.f / samplingRate);

			MetricsManager::instance()->publish(fullName, metricsValue, "ms", sampleValue);
		}
#endif

		task->release();

		task = NULL;

		while (pauseWorker) {
			Thread::sleep(1);
			Thread::yield();

			continue;
		}
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

VectorMap<String, RunStatistics> TaskWorkerThread::getBDBReadStatistics() {
	ReadLocker locker(&tasksStatsGuard);

	VectorMap<String, RunStatistics> ret = bdbReadStatistics;

	return ret;
}

VectorMap<String, RunStatistics> TaskWorkerThread::getMysqlStatistics() {
	ReadLocker locker(&tasksStatsGuard);

	VectorMap<String, RunStatistics> ret = mysqlStatistics;

	return ret;
}

void TaskWorkerThread::clearTaskStatistics() {
	Locker locker(&tasksStatsGuard);

	tasksStatistics.removeAll();
	luaTasksStatistics.removeAll();
	bdbReadStatistics.removeAll();
	mysqlStatistics.removeAll();
}

void TaskWorkerThread::setStatsDSamplingRate(int val) {
	samplingRate = val;
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

void TaskWorkerThread::addBDBReadStats(const String& dbName, uint64 elapsedTime) {
	Locker guard(&tasksStatsGuard);

	auto entry = bdbReadStatistics.find(dbName);

	RunStatistics* stats = NULL;

	if (entry == -1) {
		RunStatistics stats;

		stats.totalRunCount = 1;
		stats.totalRunTime = elapsedTime;
		stats.maxRunTime = elapsedTime;
		stats.minRunTime = elapsedTime;

		bdbReadStatistics.put(dbName, stats);
	} else {
		RunStatistics& stats = bdbReadStatistics.elementAt(entry).getValue();

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

void TaskWorkerThread::addMysqlStats(const String& query, uint64 elapsedTime) {
	Locker guard(&tasksStatsGuard);

	auto entry = mysqlStatistics.find(query);

	RunStatistics* stats = NULL;

	if (entry == -1) {
		if (mysqlStatistics.size() >= STATS_MAX_MYSQL_QUERIES)
			mysqlStatistics.remove(0);

		RunStatistics stats;

		stats.totalRunCount = 1;
		stats.totalRunTime = elapsedTime;
		stats.maxRunTime = elapsedTime;
		stats.minRunTime = elapsedTime;

		mysqlStatistics.put(query, stats);
	} else {
		RunStatistics& stats = mysqlStatistics.elementAt(entry).getValue();

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

void TaskWorkerThread::setPause(bool val) {
	pauseWorker = val;

	if (val) {
		queue->wake();
	}
}