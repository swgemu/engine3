/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_

#include "system/lang.h"

#include "Task.h"

#ifdef CXX11_COMPILER
#include "LambdaFunction.h"
#endif

namespace engine {
  namespace core {

	class TaskManager {
	public:
		virtual ~TaskManager() {

		}

		virtual void initialize() = 0;

		virtual void start() = 0;

		virtual void shutdown() = 0;

		virtual void initializeCustomQueue(const String& queueName, int numberOfThreads, bool blockDuringSaveEvent = true, bool start = true) {

		}

		virtual void setLogLevel(int level) {

		}

		virtual Vector<Locker*>* blockTaskManager() {
			return nullptr;
		}

		virtual void unblockTaskManager(Vector<Locker*>* lockers) {

		}

		virtual bool getNextExecutionTime(Task* task, Time& nextExecutionTime) = 0;

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

		virtual bool isTaskScheduled(Task* task) = 0;

		virtual void scheduleTask(Task* task, uint64 delay = 0) = 0;
		virtual void scheduleTask(Task* task, Time& time) = 0;

		virtual void scheduleIoTask(Task* task, uint64 delay = 0) {
			scheduleTask(task, delay);
		}

		virtual void scheduleIoTask(Task* task, Time& time) {
			scheduleTask(task, time);
		}

		virtual void rescheduleTask(Task* task, uint64 delay = 0) = 0;
		virtual void rescheduleTask(Task* task, Time& time) = 0;

		virtual void rescheduleIoTask(Task* task, uint64 delay = 0) {
			rescheduleTask(task, delay);
		}

		virtual void rescheduleIoTask(Task* task, Time& time) {
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
		void executeTask(std::function<void()>&& function, const char* name) {
			auto taskObject = new LambdaTask(std::move(function), name);
			taskObject->execute();
		}

		void executeTask(std::function<void()>&& function, const char* name, const char* customQueue) {
			auto taskObject = new LambdaTask(std::move(function), name);
			taskObject->setCustomTaskQueue(customQueue);
			taskObject->execute();
		}

		void executeTask(const std::function<void()>& function, const char* name) {
			auto taskObject = new LambdaTask(function, name);
			taskObject->execute();
		}

		void executeTask(const std::function<void()>& function, const char* name, const char* customQueue) {
			auto taskObject = new LambdaTask(function, name);
			taskObject->setCustomTaskQueue(customQueue);
			taskObject->execute();
		}

		void scheduleTask(std::function<void()>&& function, const char* name, uint64 delay) {
			auto taskObject = new LambdaTask(std::move(function), name);
			taskObject->schedule(delay);
		}

		void scheduleTask(std::function<void()>&& function, const char* name, uint64 delay, const char* customQueue) {
			auto taskObject = new LambdaTask(std::move(function), name);
			taskObject->setCustomTaskQueue(customQueue);
			taskObject->schedule(delay);
		}

		void scheduleTask(const std::function<void()>& function, const char* name, uint64 delay) {
			auto taskObject = new LambdaTask(function, name);
			taskObject->schedule(delay);
		}

		void scheduleTask(const std::function<void()>& function, const char* name, uint64 delay, const char* customQueue) {
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
