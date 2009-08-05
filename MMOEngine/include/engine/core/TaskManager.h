/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_

#include "system/lang.h"

#include "../log/Logger.h"

#include "../util/Singleton.h"

#include "Task.h"
#include "ReentrantTask.h"

#include "TaskQueue.h"

#include "TaskScheduler.h"

namespace engine {
  namespace core {

	class TaskWorkerThread;

	class TaskManager : public Singleton<TaskManager>, public Mutex, public Logger {
		TaskQueue tasks;

		Vector<TaskWorkerThread*> workers;
		Vector<TaskScheduler*> schedulers;

		int currentTaskScheduler;

		TaskManager();

		virtual ~TaskManager();

		static const int DEAFULT_WORKER_THREADS = 20;
		static const int DEAFULT_SCHEDULER_THREADS = 4;

	public:
		void initialize();

		void shutdown();

		void executeTask(Task* task);

		void scheduleTask(Task* task, uint64 delay = 0);
		void scheduleTask(Task* task, Time& time);

		inline void flushTasks() {
			tasks.flush();
		}

		int getScheduledTaskSize();

		int getExecutingTaskSize();

	private:
		TaskScheduler* getTaskScheduler();

		inline Task* getTask() {
			return tasks.pop();
		}

		friend class TaskWorkerThread;
		friend class SingletonWrapper<TaskManager>;
	};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /* TASKMANAGER_H_ */
