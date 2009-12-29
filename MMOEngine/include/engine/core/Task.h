#ifndef TASK_H_
#define TASK_H_

#include <signal.h>

#include "system/lang.h"

namespace engine {
  namespace core {

	class TaskScheduler;
	class TimedTaskQueue;

	class Task : public PriorityQueueEntry, public Runnable, public virtual Object {
	protected:
		TaskScheduler* taskScheduler;

		Time nextExecutionTime;

		int priority;

		bool reentrantTask;

	public:
		Task() : PriorityQueueEntry() {
			taskScheduler = NULL;

			priority = 3;

			reentrantTask = false;
		}

		Task(uint64 mtime) : PriorityQueueEntry() {
			taskScheduler = NULL;

			nextExecutionTime.addMiliTime(mtime);

			priority = 3;

			reentrantTask = false;
		}

		Task(Time& time) : PriorityQueueEntry() {
			taskScheduler = NULL;

			nextExecutionTime = time;

			priority = 3;

			reentrantTask = false;
		}

		virtual ~Task() {
			if (isQueued()) {
				System::out << "ERROR: scheduled task deleted\n";
				raise(SIGSEGV);
			}
		}

		bool cancel();

		void schedule(uint64 delay = 0);
		void schedule(Time& time);

		void reschedule(uint64 delay = 0);
		void reschedule(Time& time);

		/*int compareTo(Task* task) {
			if (priority == task->priority) {
				if (this == task)
					return 0;
				else if (this < task)
					return 1;
				else
					return -1;
			} else if (priority < task->priority) {
				return 1;
			} else
				return -1;
		}*/

		int compareTo(PriorityQueueEntry* node) {
			Task* task = (Task*) node;

			int cmp = nextExecutionTime.compareTo(task->nextExecutionTime);
			if (cmp == 0) {
				if (this == task)
					return 0;
				else if (this < task)
					return 1;
				else
					return -1;
			} else
				return cmp;
		}

		virtual String toStringData() {
			struct timespec* ts = nextExecutionTime.getTimeSpec();

			StringBuffer s;
			s << nextExecutionTime.getMiliTime() << "[" << ts->tv_sec << "/" << ts->tv_nsec << "] (ptr = " << this << ")";
			return s.toString();
		}

	protected:
		void setExecutionTime(Time& time) {
			nextExecutionTime = time;
		}

		void updateExecutionTime(uint64 mtime = 0) {
			nextExecutionTime.updateToCurrentTime();

			if (mtime != 0)
				nextExecutionTime.addMiliTime(mtime);
		}

	public:
		inline TaskScheduler* getTaskScheduler() {
			return taskScheduler;
		}

		inline Time& getNextExecutionTime() {
			return nextExecutionTime;
		}

		inline int getPriroty() {
			return priority;
		}

		inline bool isReentrant() {
			return reentrantTask;
		}

		inline bool isScheduled() {
			return taskScheduler != NULL;
		}

		inline void setTaskScheduler(TaskScheduler* scheduler) {
			taskScheduler = scheduler;
		}

		inline void setPriority(int priority) {
			Task::priority = priority;
		}

		inline void setReentrant() {
			reentrantTask = true;
		}

		inline void setNonReentrant() {
			reentrantTask = false;
		}

		friend class TimedTaskQueue;
	};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /* TASK_H_ */
