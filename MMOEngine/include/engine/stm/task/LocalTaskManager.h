/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LOCALTASKMANAGER_H_
#define LOCALTASKMANAGER_H_

#include "engine/core/Task.h"

#include "engine/core/TaskManager.h"
#include "engine/core/TaskManagerImpl.h"

namespace engine {
  namespace stm {

  	class LocalTaskManager : public TaskManager {
  		SortedVector<Task* > executedTasks;

  		SortedVector<Task*> scheduledTasks;
  		SortedVector<Task*> cancelledTasks;

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

  		bool isTaskScheduled(Task* task);

  		bool isTaskCancelled(Task* task);

		void mergeTasks(TaskManagerImpl* manager);

  		void flushTasks();

  		void printInfo();

  		int getScheduledTaskSize();

  		int getExecutingTaskSize();
  	  };

  } // namespace stm
} // namespace engine

using namespace engine::stm;

#endif /* LOCALTASKMANAGER_H_ */
