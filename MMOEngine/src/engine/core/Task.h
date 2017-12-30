#ifndef TASK_H_
#define TASK_H_

#include <signal.h>

#include "system/lang.h"

#include "system/util/Timer.h"

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
		TaskManager* taskManager;

		AtomicReference<TaskScheduler*> taskScheduler;

		Time nextExecutionTime;

		String customTaskQueue;

	#ifdef TRACE_TASKS
		StackTrace* scheduleTrace;
	#endif

		uint64 period;

	#ifdef COLLECT_TASKSTATISTICS
		uint64 lastElapsedTime;
		int statsSampleRate;
	#endif

		int priority;

	  public:
		Task();
		Task(uint64 mtime);
		Task(Time& time);

		virtual ~Task();

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
			return typeid(*this).name();
		}

		int compareTo(PriorityQueueEntry* node) {
			Task* task = (Task*) node;

			if (task == this)
				return 0;

			int cmp = nextExecutionTime.compareTo(task->nextExecutionTime);
			if (cmp == 0) {
				if (std::less<Task*>()(this, task))
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
		void initialize();

		void doExecute();

		void setExecutionTime(Time& time) {
			nextExecutionTime = time;
		}

		void updateExecutionTime(uint64 mtime = 0) {
			nextExecutionTime.updateToCurrentTime();

			if (mtime != 0)
				nextExecutionTime.addMiliTime(mtime);
		}

		inline Time& getNextExecutionTime() {
			return nextExecutionTime;
		}

	public:
		inline TaskScheduler* getTaskScheduler() {
			return taskScheduler;
		}

		inline int getPriroty() {
			return priority;
		}

		inline bool isPeriodic() {
			return period != 0;
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

		inline uint64 getPeriod() {
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

	#ifdef COLLECT_TASKSTATISTICS
		void setStatsSample(bool val) {
			statsSampleRate = val;
		}

	  	inline uint64 getLastElapsedTime() const {
	  		return lastElapsedTime;
	  	}

		inline int getStatsSampleRate() const {
			return statsSampleRate;
		}
	#endif

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

#endif /* TASK_H_ */
