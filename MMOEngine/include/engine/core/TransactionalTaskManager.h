/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TRANSACTIONALTASKMANAGER_H_
#define TRANSACTIONALTASKMANAGER_H_

#include "engine/util/Command.h"

#include "TaskQueue.h"

#include "TaskManager.h"
#include "TaskManagerImpl.h"

namespace engine {
  namespace core {

  	class TransactionalTaskManager : public TaskManager, public Command {
  		TaskManagerImpl* taskManager;

  		ThreadLocal<TaskManagerImpl>  localTaskManager;

  	  public:
  		void initialize();

  		void start();

  		void shutdown();

  		void executeTask(Task* task);

  		void scheduleTask(Task* task, uint64 delay);
  		void scheduleTask(Task* task, Time& time);

  		void rescheduleTask(Task* task, uint64 delay);
  		void rescheduleTask(Task* task, Time& time);

  		bool cancelTask(Task* task);

  		Task* getTask();

  		void flushTasks();

  		void printInfo();

  		int getScheduledTaskSize();

  		int getExecutingTaskSize();

  		void execute();

  		void undo();

  	  protected:
  		TaskManagerImpl* getLocalTaskManager();

  	  };

  } // namespace service
} // namespace engine

using namespace engine::core;

#endif /* TASKMANAGER_H_ */
