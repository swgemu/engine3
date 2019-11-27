/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_

#include "system/lang.h"
#include "system/thread/atomic/AtomicTime.h"
#include "system/lang/Pair.h"
#include "system/lang/Function.h"

#include "Task.h"

#ifdef CXX11_COMPILER
#include "LambdaFunction.h"
#endif

namespace engine {
  namespace core {
	class TaskQueue;
	class TaskWorkerThread;

	class TaskManager {
	public:
		virtual ~TaskManager() {

		}

		virtual void initialize() = 0;

		virtual void start() = 0;

		virtual void shutdown() = 0;

		virtual TaskQueue* initializeCustomQueue(const String& queueName, int numberOfThreads, bool blockDuringSaveEvent = true, bool start = true) {
			return nullptr;
		}

		virtual TaskQueue* getCustomTaskQueue(const String& queueName) {
			return nullptr;
		}

		virtual void waitForQueueToFinish(const String& queueName) {

		}

		virtual void initalizeDatabaseHandles() {

		}

		virtual void setLogLevel(int level) {

		}

		virtual Vector<Pair<Locker*, TaskWorkerThread*>>* blockTaskManager() {
			return nullptr;
		}

		virtual void unblockTaskManager(Vector<Pair<Locker*, TaskWorkerThread*>>* lockers) {

		}

		virtual bool getNextExecutionTime(const Task* task, AtomicTime& nextExecutionTime) = 0;

		virtual void executeTask(Task* task) = 0;

		virtual void executeTask(Task* task, int taskqueue) {
		        executeTask(task);
		}

#ifdef WITH_STM
		virtual void retryTaskInSerial(Task* task) {

		}

		virtual Thread* getSerialWorker() {
			return nullptr;
		}
#endif

		virtual bool isTaskScheduled(const Task* task) = 0;

		virtual void scheduleTask(Task* task, uint64 delay = 0) = 0;
		virtual void scheduleTask(Task* task, const Time& time) = 0;

		virtual void scheduleIoTask(Task* task, uint64 delay = 0) {
			scheduleTask(task, delay);
		}

		virtual void scheduleIoTask(Task* task, const Time& time) {
			scheduleTask(task, time);
		}

		virtual void rescheduleTask(Task* task, uint64 delay = 0) = 0;
		virtual void rescheduleTask(Task* task, const Time& time) = 0;

		virtual void rescheduleIoTask(Task* task, uint64 delay = 0) {
			rescheduleTask(task, delay);
		}

		virtual void rescheduleIoTask(Task* task, const Time& time) {
			rescheduleTask(task, time);
		}

		virtual bool cancelTask(Task* task) = 0;

		//virtual Task* getTask() = 0;

		virtual void flushTasks() = 0;

		virtual String getInfo(bool print) = 0;

		virtual int getScheduledTaskSize() = 0;

		virtual int getExecutingTaskSize() = 0;

		virtual void clearWorkersTaskStats() {

		}

		virtual void setStatsDTaskSampling(int val) {

		}

		virtual void setStatsDBdbSamplingRate(int val) {

		}

		virtual Task* getCurrentThreadTask() {
			return nullptr;
		}

#ifdef CXX11_COMPILER
		void executeTask(Function<void()>&& function, const char* name) {
			auto taskObject = new LambdaTask(std::move(function), name);
			taskObject->execute();
		}

		void executeTask(Function<void()>&& function, const char* name, const char* customQueue) {
			auto taskObject = new LambdaTask(std::move(function), name);
			taskObject->setCustomTaskQueue(customQueue);
			taskObject->execute();
		}

		void executeTask(const Function<void()>& function, const char* name) {
			auto taskObject = new LambdaTask(function, name);
			taskObject->execute();
		}

		void executeTask(const Function<void()>& function, const char* name, const char* customQueue) {
			auto taskObject = new LambdaTask(function, name);
			taskObject->setCustomTaskQueue(customQueue);
			taskObject->execute();
		}

		void scheduleTask(Function<void()>&& function, const char* name, uint64 delay) {
			auto taskObject = new LambdaTask(std::move(function), name);
			taskObject->schedule(delay);
		}

		void scheduleTask(Function<void()>&& function, const char* name, uint64 delay, const char* customQueue) {
			auto taskObject = new LambdaTask(std::move(function), name);
			taskObject->setCustomTaskQueue(customQueue);
			taskObject->schedule(delay);
		}

		void scheduleTask(const Function<void()>& function, const char* name, uint64 delay) {
			auto taskObject = new LambdaTask(function, name);
			taskObject->schedule(delay);
		}

		void scheduleTask(const Function<void()>& function, const char* name, uint64 delay, const char* customQueue) {
			auto taskObject = new LambdaTask(function, name);
			taskObject->setCustomTaskQueue(customQueue);
			taskObject->schedule(delay);
		}
#endif


	  };

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /* TASKMANAGER_H_ */
