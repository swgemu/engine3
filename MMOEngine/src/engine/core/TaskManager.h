/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_

#include "system/lang.h"

#include "../log/Logger.h"

#include "../util/Singleton.h"

#include "TaskQueue.h"

namespace engine {
  namespace core {

	class TaskWorkerThread;
	class TaskScheduler;
	class Task;

	class TaskManager : public Singleton<TaskManager>, public Mutex, public Logger {
		TaskQueue tasks;

		Vector<TaskWorkerThread*> workers;
		Vector<TaskScheduler*> schedulers;

		int currentTaskScheduler;

		TaskManager();

		virtual ~TaskManager();

		static const int DEAFULT_WORKER_THREADS = 1;
		static const int DEAFULT_SCHEDULER_THREADS = 1;

	public:
		void initialize();

		void shutdown();

		void executeTask(Task* task);

		void scheduleTask(Task* task, uint64 delay = 0);
		void scheduleTask(Task* task, Time& time);

		void rescheduleTask(Task* task, uint64 delay = 0);
		void rescheduleTask(Task* task, Time& time);

		bool cancelTask(Task* task);

		inline void flushTasks() {
			tasks.flush();
		}

		void testScheduler();

		void printInfo();

		int getScheduledTaskSize();

		int getExecutingTaskSize();

	private:
		TaskScheduler* getTaskScheduler(bool doLock = true);

		void setTaskScheduler(Task* task, TaskScheduler* scheduler, bool doLock = true);

		inline Task* getTask() {
			return tasks.pop();
		}

		friend class TaskScheduler;
		friend class TaskWorkerThread;
		friend class SingletonWrapper<TaskManager>;
	};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /* TASKMANAGER_H_ */
