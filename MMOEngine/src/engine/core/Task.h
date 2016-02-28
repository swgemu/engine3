#ifndef TASK_H_
#define TASK_H_

#include <boost/typeof/typeof.hpp>

#include <signal.h>

#include "system/lang.h"

#include "system/util/Timer.h"

//#define COLLECT_TASKSTATISTICS

#define EXECUTE_TASK_1(Param1, Code) \
        { \
        class Param1##Param1 : public Task { \
                typedef BOOST_TYPEOF_TPL(Param1) param1_type; \
                param1_type Param1##_p; \
        public: \
                Param1##Param1(param1_type param1) : Param1##_p(param1) {} \
                void run() { \
                        Code; \
                } \
        }; \
        Param1##Param1* task = new Param1##Param1(Param1); \
        task->execute(); \
        }


#define EXECUTE_TASK_2(Param1, Param2, Code) \
        { \
        class Param1##Param2 : public Task { \
                typedef BOOST_TYPEOF_TPL(Param1) param1_type; \
                param1_type Param1##_p; \
		\
				typedef BOOST_TYPEOF_TPL(Param2) param2_type; \
				param2_type Param2##_p; \
        public: \
                Param1##Param2(param1_type param1, param2_type param2) : Param1##_p(param1), Param2##_p(param2) {} \
                void run() { \
                        Code; \
                } \
        }; \
        Param1##Param2* task = new Param1##Param2(Param1, Param2); \
        task->execute(); \
        }


#define EXECUTE_TASK_3(Param1, Param2, Param3, Code) \
        { \
        class Param1##Param2##Param3 : public Task { \
                typedef BOOST_TYPEOF_TPL(Param1) param1_type; \
                param1_type Param1##_p; \
		\
                typedef BOOST_TYPEOF_TPL(Param2) param2_type; \
                param2_type Param2##_p; \
		\
				typedef BOOST_TYPEOF_TPL(Param3) param3_type; \
				param3_type Param3##_p; \
        public: \
                Param1##Param2##Param3(param1_type param1, param2_type param2, param3_type param3) : Param1##_p(param1), Param2##_p(param2), Param3##_p(param3) {} \
                void run() { \
                        Code; \
                } \
        }; \
        Task* task = new Param1##Param2##Param3(Param1, Param2, Param3); \
        task->execute(); \
        }


#define EXECUTE_TASK_4(Param1, Param2, Param3, Param4, Code) \
        { \
        class Param1##Param2##Param3##Param4 : public Task { \
                typedef BOOST_TYPEOF_TPL(Param1) param1_type; \
                param1_type Param1##_p; \
		\
                typedef BOOST_TYPEOF_TPL(Param2) param2_type; \
                param2_type Param2##_p; \
		\
                typedef BOOST_TYPEOF_TPL(Param3) param3_type; \
                param3_type Param3##_p; \
		\
				typedef BOOST_TYPEOF_TPL(Param4) param4_type; \
				param4_type Param4##_p; \
        public: \
                Param1##Param2##Param3##Param4(param1_type param1, param2_type param2, param3_type param3, param4_type param4) : Param1##_p(param1), Param2##_p(param2), Param3##_p(param3), Param4##_p(param4) {} \
                void run() { \
                        Code; \
                } \
        }; \
        Task* task = new Param1##Param2##Param3##Param4(Param1, Param2, Param3, Param4); \
        task->execute(); \
        }

#define EXECUTE_TASK_5(Param1, Param2, Param3, Param4, Param5, Code) \
        { \
        class Param1##Param2##Param3##Param4##Param5 : public Task { \
                typedef BOOST_TYPEOF_TPL(Param1) param1_type; \
                param1_type Param1##_p; \
		\
                typedef BOOST_TYPEOF_TPL(Param2) param2_type; \
                param2_type Param2##_p; \
		\
                typedef BOOST_TYPEOF_TPL(Param3) param3_type; \
                param3_type Param3##_p; \
		\
				typedef BOOST_TYPEOF_TPL(Param4) param4_type; \
				param4_type Param4##_p; \
		\
				typedef BOOST_TYPEOF_TPL(Param5) param5_type; \
                param5_type Param5##_p; \
		\
        public: \
                Param1##Param2##Param3##Param4##Param5(param1_type param1, param2_type param2, param3_type param3, param4_type param4, param5_type param5) : Param1##_p(param1), Param2##_p(param2), Param3##_p(param3), Param4##_p(param4), Param5##_p(param5) {} \
                void run() { \
                        Code; \
                } \
        }; \
        Task* task = new Param1##Param2##Param3##Param4##Param5(Param1, Param2, Param3, Param4, Param5); \
        task->execute(); \
        }

#define EXECUTE_TASK_6(Param1, Param2, Param3, Param4, Param5, Param6, Code) \
        { \
        class Param1##Param2##Param3##Param4##Param5##Param6 : public Task { \
                typedef BOOST_TYPEOF_TPL(Param1) param1_type; \
                param1_type Param1##_p; \
		\
                typedef BOOST_TYPEOF_TPL(Param2) param2_type; \
                param2_type Param2##_p; \
		\
                typedef BOOST_TYPEOF_TPL(Param3) param3_type; \
                param3_type Param3##_p; \
		\
				typedef BOOST_TYPEOF_TPL(Param4) param4_type; \
				param4_type Param4##_p; \
		\
				typedef BOOST_TYPEOF_TPL(Param5) param5_type; \
                param5_type Param5##_p; \
		\
				typedef BOOST_TYPEOF_TPL(Param6) param6_type; \
                param6_type Param6##_p; \
		\
        public: \
                Param1##Param2##Param3##Param4##Param5##Param6(param1_type param1, param2_type param2, param3_type param3, param4_type param4, param5_type param5, param6_type param6) : Param1##_p(param1), Param2##_p(param2), Param3##_p(param3), Param4##_p(param4), Param5##_p(param5), Param6##_p(param6) {} \
                void run() { \
                        Code; \
                } \
        }; \
        Task* task = new Param1##Param2##Param3##Param4##Param5##Param6(Param1, Param2, Param3, Param4, Param5, Param6); \
        task->execute(); \
        }

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
		class TaskManager* taskManager;

		AtomicReference<TaskScheduler*> taskScheduler;

		Time nextExecutionTime;

		int priority;

		uint64 period;

	#ifdef COLLECT_TASKSTATISTICS
		Timer queuedTimer;
		Timer executionTimer;
		Timer blockedTimer;
	#endif

	#ifdef TRACE_TASKS
		StackTrace* scheduleTrace;
	#endif

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
			/*	if (this == task)
					return 0;
				else */if (this < task)
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
			return taskScheduler.compareAndSet(NULL, scheduler);
		}

		inline bool clearTaskScheduler() {
			TaskScheduler* scheduler = taskScheduler.get();

			return taskScheduler.compareAndSet(scheduler, NULL);
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

	#ifdef TRACE_TASKS
		void setScheduleTrace() {
			if (scheduleTrace != NULL)
				delete scheduleTrace;

			scheduleTrace = new StackTrace();
		}

		void printScheduleTrace() {
			assert(scheduleTrace != NULL);

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
