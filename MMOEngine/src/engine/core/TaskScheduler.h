/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TASKSCHEDULER_H_
#define TASKSCHEDULER_H_

#include "system/lang.h"

#include "../log/Logger.h"

#include "Task.h"
#include "ReentrantTask.h"

#include "TimedTaskQueue.h"

namespace engine {
  namespace core {

	class TaskScheduler : public Thread, public Logger {
		TimedTaskQueue tasks;

		bool doRun;

	public:
		TaskScheduler();
		TaskScheduler(const String& s);

		virtual ~TaskScheduler();

		void start();

		void run();

		void stop();

		inline bool scheduleTask(Task* task, uint64 delay = 0) {
			if (task->isQueued())
				return true;

			return tasks.add(task, delay);
		}

		inline bool scheduleTask(Task* task, Time& time) {
			if (task->isQueued())
				return true;

			return tasks.add(task, time);
		}

		inline bool rescheduleTask(Task* task, uint64 delay = 0) {
			return tasks.add(task, delay);
		}

		inline bool rescheduleTask(Task* task, Time& time) {
			return tasks.add(task, time);
		}

		inline bool cancelTask(Task* task) {
			return tasks.remove(task);
		}

		void fixQueue() {
			tasks.repair();
		}

		inline int getQueueSize() {
			return tasks.size();
		}

		inline void printTasks() {
			tasks.printQueue();
		}
	};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /* TASKSCHEDULER_H_ */
