/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TASKMANAGERIMPL_H_
#define TASKMANAGERIMPL_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "TaskManager.h"
#include "TaskQueue.h"

namespace engine {
  namespace core {

	class TaskWorkerThread;
	class TaskScheduler;
	class Task;

	class TaskManagerImpl : public TaskManager, public Mutex, public Logger {
		Vector<TaskQueue*> taskQueues;

		Vector<Reference<TaskWorkerThread*> > workers;
		Vector<Reference<TaskScheduler*> > schedulers;

		Vector<Reference<TaskScheduler*> > ioSchedulers;

		VectorMap<String, int> customQueues;

#ifdef WITH_STM
		TaskWorkerThread* serialWorker;
		TaskQueue serialTaskQueue;
#endif

		int workerThreads;
		int schedulerThreads;

		AtomicInteger currentTaskScheduler;
        AtomicInteger currentTaskQueue;

		bool shuttingDown;

	public:
	    static int DEFAULT_WORKER_QUEUES;
		static int DEFAULT_WORKER_THREADS_PER_QUEUE;
		static int DEFAULT_SCHEDULER_THREADS;
		static int DEFAULT_IO_SCHEDULERS;

		TaskManagerImpl();

		virtual ~TaskManagerImpl();

		void initialize();

		void initialize(int workerCount, int schedulerCount, int ioCount);

		void initializeCustomQueue(const String& queueName, int numberOfThreads, bool blockDuringSaveEvent = true, bool start = true);

		void start();

		void shutdown();

		void clearWorkersTaskStats();
		void setStatsDTaskSampling(int val);
		void setStatsDBdbSamplingRate(int val);

		Vector<Locker*>* blockTaskManager();
		void unblockTaskManager(Vector<Locker*>* lockers);

#ifdef WITH_STM
		void retryTaskInSerial(Task* task);

		Thread* getSerialWorker();
#endif

		void executeTask(Task* task);
		void executeTask(Task* task, int taskQueue);
		void executeTask(Task* task, const String& customTaskQueue);

		void executeTasks(const Vector<Task*>& tasks);

		void executeTaskFront(Task* task);
		void executeTaskRandom(Task* task);
		bool getNextExecutionTime(Task* task, Time& nextExecutionTime);

		bool isTaskScheduled(Task* task);

		void scheduleTask(Task* task, uint64 delay = 0);
		void scheduleTask(Task* task, Time& time);

		void scheduleIoTask(Task* task, uint64 delay = 0);
		void scheduleIoTask(Task* task, Time& time);

		void rescheduleTask(Task* task, uint64 delay = 0);
		void rescheduleTask(Task* task, Time& time);

		void rescheduleIoTask(Task* task, uint64 delay = 0);
		void rescheduleIoTask(Task* task, Time& time);

		bool cancelTask(Task* task);

		void flushTasks();

		void testScheduler();

		String getInfo(bool print);

		int getScheduledTaskSize();

		int getIoScheduledTaskSize();

		int getExecutingTaskSize();

		void setLogLevel(int level);

		Task* getCurrentThreadTask();
	private:
		TaskScheduler* getTaskScheduler();
		TaskScheduler* getIoTaskScheduler();

		void setTaskScheduler(Task* task, TaskScheduler* scheduler);

		/*inline Task* getTask() {
			return tasks.pop();
		}*/

		friend class TaskScheduler;
		friend class TaskWorkerThread;
	};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /* TASKMANAGERIMPL_H_ */
