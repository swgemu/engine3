/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TASKWORKERTHREAD_H_
#define TASKWORKERTHREAD_H_

#include "engine/service/ServiceThread.h"

#include "RunStatistics.h"

namespace engine {
  namespace core {

	class TaskManager;
	class TaskQueue;

	class TaskWorkerThread : public ServiceThread {
		Mutex blockMutex;
		TaskQueue* queue;
		int cpu;

		bool blockDuringSave;

#ifdef COLLECT_TASKSTATISTICS
		HashTable<const char*, RunStatistics> tasksStatistics;
		VectorMap<String, RunStatistics> luaTasksStatistics;
		VectorMap<String, RunStatistics> bdbReadStatistics;
		VectorMap<String, RunStatistics> mysqlStatistics;

		ReadWriteLock tasksStatsGuard;
#endif

	public:
		TaskWorkerThread(const String& s, TaskQueue* queue, int cpu, bool blockDuringSave);

		virtual ~TaskWorkerThread();

		void start();

		//void pushToRetryQueue(Task* task);

		virtual void run();

		void stop();

#ifdef COLLECT_TASKSTATISTICS
		HashTable<const char*, RunStatistics> getTasksStatistics();
		VectorMap<String, RunStatistics> getLuaTasksStatistics();
		VectorMap<String, RunStatistics> getBDBReadStatistics();
		VectorMap<String, RunStatistics> getMysqlStatistics();

		void addLuaTaskStats(const String& name, uint64 runTime);
		void addBDBReadStats(const String& dbName, uint64 runTime);
		void addMysqlStats(const String& query, uint64 runTime);

		void clearTaskStatistics();

#ifdef CXX11_COMPILER
		void addLuaTaskStats(String&& name, uint64 runTime);
#endif
#endif

		TaskWorkerThread* asTaskWorkerThread() {
			return this;
		}

		inline Mutex* getBlockMutex() {
			return &blockMutex;
		}

		inline bool doBlockWorkerDuringSave() const {
			return blockDuringSave;
		}
	};

  } // namespace service
} // namespace engine

using namespace engine::core;

#endif /* TASKWORKERTHREAD_H_ */
