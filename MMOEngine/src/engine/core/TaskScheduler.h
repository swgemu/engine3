/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TASKSCHEDULER_H_
#define TASKSCHEDULER_H_

#include "system/lang.h"

#include "system/util/SynchronizedHashTable.h"

#include "engine/log/Logger.h"

#include "Task.h"

#include "TimedTaskQueue.h"

namespace engine {
  namespace core {

	class TaskScheduler : public Thread, public Object, public Logger {
		TaskManager* taskManager;

		TimedTaskQueue tasks;

		bool doRun;

		Mutex blockMutex;

		HashTable<const char*, uint64> tasksCount;
		ReadWriteLock tasksCountGuard;
		volatile bool pause;

	protected:
		void prepareTask(Task* task);

	public:
		TaskScheduler();
		TaskScheduler(const String& s);

		virtual ~TaskScheduler();

		void start();

		void run();

		void stop();

		void setPause(bool val);

		bool scheduleTask(Task* task, uint64 delay = 0);
		bool scheduleTask(Task* task, Time& time);

		bool cancelTask(Task* task);

		void addSchedulerTasks(TaskScheduler* scheduler);

		HashTable<const char*, uint64> getTasksCount();

		void flushTasks() {
			tasks.flush();
		}

		void clearTasks() {
			tasks.clear();
		}

		void fixQueue() {
			tasks.repair();
		}

		inline int getQueueSize() {
			return tasks.size();
		}

		inline uint64 getPushedTasks() {
			return tasks.getPushedEntries();
		}

		inline uint64 getPoppedTasks() {
			return tasks.getPoppedEntries();
		}

		inline uint64 getRemovedTasks() {
			return tasks.getRemovedEntries();
		}

		inline void printTasks() {
			tasks.printQueue();
		}

		inline TaskManager* getTaskManager() {
			return taskManager;
		}

		inline void setTaskManager(TaskManager* manager) {
			taskManager = manager;
		}

		inline Mutex* getBlockMutex() {
			return &blockMutex;
		}
	};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /* TASKSCHEDULER_H_ */
