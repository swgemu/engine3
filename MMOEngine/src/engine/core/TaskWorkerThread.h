/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef TASKWORKERTHREAD_H_
#define TASKWORKERTHREAD_H_

#include "engine/service/ServiceThread.h"

#include "RunStatistics.h"

namespace engine {
  namespace core {

	class TaskManager;
	class TaskQueue;
	class Task;
	class ManagedObject;

	class TaskWorkerThread : public ServiceThread {
		Mutex blockMutex;
		TaskQueue* queue;
		int cpu;

		bool blockDuringSave;

		AtomicBoolean pauseWorker;
		AtomicBoolean initializeDBHandles;

		Task* currentTask;

		uint64 mutexWaitTime;
		uint64 mutexesAcquired;
		uint64 totalBdbTime;
		uint64 totalBdbQueries;

#ifdef COLLECT_TASKSTATISTICS
		HashTable<String, RunStatistics> tasksStatistics;
		VectorMap<String, RunStatistics> luaTasksStatistics;
		VectorMap<String, RunStatistics> bdbReadStatistics;
		VectorMap<String, RunStatistics> mysqlStatistics;

		ReadWriteLock tasksStatsGuard;

		uint64 totalTaskRunCount;
		uint64 totalBdbReadCount;

		int samplingRate;
		int bdbSamplingRate;
#endif

	public:
		TaskWorkerThread(const String& s, TaskQueue* queue, int cpu, bool blockDuringSave);

		virtual ~TaskWorkerThread();

		void start();

		//void pushToRetryQueue(Task* task);

		virtual void run();

		void stop();

		void signalDBHandleInitialize();

#ifdef COLLECT_TASKSTATISTICS
		HashTable<String, RunStatistics> getTasksStatistics();
		VectorMap<String, RunStatistics> getLuaTasksStatistics();
		VectorMap<String, RunStatistics> getBDBReadStatistics();
		VectorMap<String, RunStatistics> getMysqlStatistics();

		void addLuaTaskStats(const String& name, uint64 runTime);
		void addMysqlStats(const String& query, uint64 runTime);

		void clearTaskStatistics();
		void setStatsDSamplingRate(int val);
		void setStatsDBdbSamplingRate(int val);

#ifdef CXX11_COMPILER
		void addLuaTaskStats(String&& name, uint64 runTime);
#endif
#endif
		void addBDBReadStats(const String& dbName, uint64 runTime);

		void setPause(bool val);

		void logTask(const char* taskName, uint64 elapsedTime) const;

		inline void addMutexWaitTime(uint64 time) {
			mutexesAcquired++;
			mutexWaitTime += time;
		}

		void clearMutexWaitTime() {
			mutexWaitTime = 0;
		}

		uint64 getTotalBdbTime() const {
			return totalBdbTime;
		}

		uint64 getMutexWaitTime() const {
			return mutexWaitTime;
		}

		TaskQueue* getTaskQueue() const {
			return queue;
		}

		TaskWorkerThread* asTaskWorkerThread() {
			return this;
		}

		inline Mutex* getBlockMutex() {
			return &blockMutex;
		}

		inline bool doBlockWorkerDuringSave() const {
			return blockDuringSave;
		}

		inline Task* getCurrentTask() const {
			return currentTask;
		}

	};

  } // namespace service
} // namespace engine

using namespace engine::core;

#endif /* TASKWORKERTHREAD_H_ */
