/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef TIMEDTASKQUEUE_H_
#define TIMEDTASKQUEUE_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "Task.h"

namespace engine {
  namespace core {

	class TaskScheduler;

	class TimedTaskQueue : public PriorityQueue, public Condition, public Logger {
		TaskScheduler* taskScheduler;

		Mutex* condMutex;

		bool blocked;

		bool waitingForTask;
		bool changePlan;

	public:
		TimedTaskQueue();

		~TimedTaskQueue();

		bool add(Task* e, uint64 delay);
		bool add(Task* e, const Time& time);

		bool add(Task* e, bool doLock = true);

		bool addAll(PriorityQueue& queue, bool doLock = true);

		Task* get();

		bool remove(Task* e, bool doLock = true);

		void flush();

		void repair();

		void printQueue();

		void wake();

		// setters and getters
		inline void setTaskScheduler(TaskScheduler* scheduler) {
			taskScheduler = scheduler;
		}

		inline void setMutexName(const String& s) {
			condMutex->setLockName(s);
		}
	};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /* TIMEDTASKQUEUE_H_ */
