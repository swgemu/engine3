/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#pragma once

#include <csignal>

#include "system/lang.h"

#include "system/util/Timer.h"
#include "system/thread/atomic/AtomicTime.h"

//#define COLLECT_TASKSTATISTICS

namespace engine {

  namespace stm {
  	  class LocalTaskManager;
  	  class TransactionalTaskManager;
  }

  namespace core {

	class TaskManager;
	class TaskScheduler;
	class TimedTaskQueue;
	class TaskManagerImpl;

	class Task : public PriorityQueueEntry, public Runnable, public Object {
	protected:
		AtomicReference<TaskScheduler*> taskScheduler;

		AtomicTime nextExecutionTime;

		String customTaskQueue;

	#ifdef TRACE_TASKS
		StackTrace* scheduleTrace;
	#endif

		AtomicLong period;

	#ifdef COLLECT_TASKSTATISTICS
		int statsSampleRate;
	#endif

		uint64 lastElapsedTime = 0;
		String taskName;

		int priority;
		bool runInScheduler = false;

	  public:
		Task();
		Task(uint64 mtime);
		Task(const Time& time);

		~Task();

		bool isScheduled();

		bool cancel();
		bool cancelNonTransactionally();

		void schedule(uint64 delay = 0);
		void schedule(Time& time);

		void scheduleInIoScheduler(uint64 delay = 0);
		void scheduleInIoScheduler(Time& time);

		void scheduleNonTransactionally(uint64 delay = 0);

		void schedulePeriodic(uint64 delay, uint64 period);
		void schedulePeriodic(Time& time, uint64 period);

		void reschedule(uint64 delay = 0);
		void rescheduleNonTransactionally(uint64 delay = 0);
		void reschedule(Time& time);

		void rescheduleInIoScheduler(uint64 delay = 0);
		void rescheduleInIoScheduler(Time& time);

		void execute();
		void executeInThread();

		virtual const char* getTaskName() {
			if (!taskName.isEmpty()) {
				return taskName.toCharArray();
			} else {
				return typeid(*this).name();
			}
		}

		int compareTo(const PriorityQueueEntry* node) const {
			const Task* task = static_cast<const Task*>(node);

			if (task == this)
				return 0;

			int cmp = nextExecutionTime.compareTo(task->nextExecutionTime);
			if (cmp == 0) {
				if (std::less<const Task*>()(this, task))
					return 1;
				else
					return -1;
			} else
				return cmp;
		}

		virtual String toStringData() const {
			StringBuffer s;
			s << nextExecutionTime.getMiliTime() << "(ptr = " << this << ")";
			return s.toString();
		}

	protected:
		void initialize();

		void doExecute();

		void setExecutionTime(const Time& time) {
			nextExecutionTime = time;
		}

		void updateExecutionTime(uint64 mtime = 0) {
			nextExecutionTime.updateToCurrentTime();

			if (mtime != 0)
				nextExecutionTime.addMiliTime(mtime);
		}

		inline const AtomicTime& getNextExecutionTime() const {
			return nextExecutionTime;
		}

	public:
		inline TaskScheduler* getTaskScheduler() {
			return taskScheduler;
		}

		inline const TaskScheduler* getTaskScheduler() const {
			return taskScheduler;
		}

		inline int getPriroty() const {
			return priority;
		}

		inline bool isPeriodic() const {
			return period != 0;
		}

		inline bool doRunInScheduler() const {
			return runInScheduler;
		}

		inline void setRunInScheduler(bool val) {
			runInScheduler = val;
		}

		inline bool setTaskScheduler(TaskScheduler* scheduler) {
			return taskScheduler.compareAndSet(nullptr, scheduler);
		}

		inline bool clearTaskScheduler() {
			TaskScheduler* scheduler = taskScheduler.get();

			return taskScheduler.compareAndSet(scheduler, nullptr);
		}

		inline void setPriority(int priority) {
			Task::priority = priority;
		}

		inline uint64 getPeriod() const {
			return period;
		}

		inline void setPeriod(uint64 per) {
			period = per;
		}

		inline void setCustomTaskQueue(const String& queue) {
			customTaskQueue = queue;
		}

		inline void setCustomTaskQueue(String&& queue) {
			customTaskQueue = std::move(queue);
		}

		inline const String& getCustomTaskQueue() const {
			return customTaskQueue;
		}

		inline void setTaskName(const char* name) {
			taskName = name;
		}

	#ifdef COLLECT_TASKSTATISTICS
		void setStatsSample(bool val) {
			statsSampleRate = val;
		}

		inline int getStatsSampleRate() const {
			return statsSampleRate;
		}
	#endif

		inline uint64 getLastElapsedTime() const {
	  		return lastElapsedTime;
	  	}

	#ifdef TRACE_TASKS
		void setScheduleTrace() {
			if (scheduleTrace != nullptr)
				delete scheduleTrace;

			scheduleTrace = new StackTrace();
		}

		void printScheduleTrace() {
			assert(scheduleTrace != nullptr);

			scheduleTrace->print();
		}
	#endif

		friend class TimedTaskQueue;
		friend class TaskScheduler;
		friend class TaskWorkerThread;
		friend class engine::stm::LocalTaskManager;
		friend class engine::stm::TransactionalTaskManager;
		friend class engine::core::Core;
		friend class engine::core::TaskManagerImpl;
	};

  } // namespace core
} // namespace engine

using namespace engine::core;

