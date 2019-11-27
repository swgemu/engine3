/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef TRANSACTIONALTASKMANAGER_H_
#define TRANSACTIONALTASKMANAGER_H_

#include "engine/util/Command.h"

#include "engine/core/TaskQueue.h"

#include "engine/core/TaskManager.h"

#include "engine/stm/task/LocalTaskManager.h"

namespace engine {
  namespace stm {

  	class TransactionalTaskManager : public TaskManager, public Command {
  		TaskManagerImpl* taskManager;

  		ThreadLocal<LocalTaskManager*>  localTaskManager;

  	  public:
  		static const int WORKER_THREADS = 2;
  		static const int SCHEDULER_THREADS = 5;

  		void initialize();

  		void start();

  		void shutdown();

  		void executeTask(Task* task);

#ifdef WITH_STM
  		void retryTaskInSerial(Task* task);

  		Thread* getSerialWorker();
#endif

  		void scheduleTask(Task* task, uint64 delay);
  		void scheduleTask(Task* task, const Time& time);

  		void rescheduleTask(Task* task, uint64 delay);
  		void rescheduleTask(Task* task, const Time& time);

  		Vector<Pair<Locker*, TaskWorkerThread*>>* blockTaskManager();
  		void unblockTaskManager(Vector<Pair<Locker*, TaskWorkerThread*>>* lockers);

  		bool cancelTask(Task* task);

  		Task* getTask();

  		inline TaskManagerImpl* getTaskManagerImpl() {
  			return taskManager;
  		}

  		bool isTaskScheduled(const Task* task);

  		bool getNextExecutionTime(const Task* task, AtomicTime& nextExecutionTime);

  		void flushTasks();

  		String getInfo(bool print);

  		int getScheduledTaskSize();

  		int getExecutingTaskSize();

  		void execute();

  		void undo();

  	  protected:
  		LocalTaskManager* getLocalTaskManager();

  	  };

  } // namespace stm
} // namespace engine

using namespace engine::stm;

#endif /* TASKMANAGER_H_ */
