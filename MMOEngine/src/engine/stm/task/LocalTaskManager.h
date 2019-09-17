/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef LOCALTASKMANAGER_H_
#define LOCALTASKMANAGER_H_

#include "engine/core/Task.h"

#include "engine/core/TaskManager.h"
#include "engine/core/TaskManagerImpl.h"

namespace engine {
  namespace stm {

   class TaskReference {
	   Task* task;

   public:
	   TaskReference() {
		   task = nullptr;
	   }

	   TaskReference(Task* t) {
		   task = t;
	   }

	   TaskReference(const TaskReference& t) {
		   task = t.task;
	   }

	   bool toBinaryStream(ObjectOutputStream* stream) {
		   return false;
	   }

	   /*static bool parseFromString(T* address, const sys::lang::String& value, int version = 0) {
	   		return address->parseFromString(value, version);
	   	}*/

	   bool parseFromBinaryStream(ObjectInputStream* stream) {
		   return false;
	   }

	   TaskReference& operator=(const TaskReference& t)  {
		   if (this == &t)
			   return *this;

		   task = t.task;

		   return *this;
	   }

	   int compareTo(const TaskReference& ref) const {
		   if (task == ref.task)
			   return 0;
		   else if (task < ref.task)
			   return 1;
		   else
			   return -1;
	   }

	   operator Task*() {
		   return task;
	   }

	   Task* operator->() {
		   return task;
	   }

	   inline Task* getTask() {
		   return task;
	   }


   };

    class TaskAction : public Object {
    	int type;
    	Reference<Task*> task;
    	AtomicTime nextExecutionTime;

    public:
    	const static int CANCEL = 1;
    	const static int SCHEDULE = 2;
    	const static int EXECUTE = 3;
    	const static int RESCHEDULE = 4;

    public:
    	TaskAction(const TaskAction& a) : Object() {
    		type = a.type;
    		task = a.task;
    		nextExecutionTime = a.nextExecutionTime;
    	}

    	TaskAction(int type, Reference<Task*> task, const AtomicTime& nextExecutionTime) {
    		TaskAction::type = type;
    		TaskAction::task = task;
    		TaskAction::nextExecutionTime = nextExecutionTime;
    	}

    	TaskAction& operator=(const TaskAction& a) {
    		if (this == &a)
    			return *this;

    		type = a.type;
    		task = a.task;
    		nextExecutionTime = a.nextExecutionTime;

    		return *this;
    	}

    	inline int getType() {
    		return type;
    	}

    	inline Task* getTask() {
    		return task;
    	}

    	inline AtomicTime& getNextExecutionTime() {
    		return nextExecutionTime;
    	}

    };

  	class LocalTaskManager : public TaskManager {
  		Vector<Task*> executedTasks;

  		/*SortedVector<Task*> scheduledTasks;
  		SortedVector<Task*> cancelledTasks;*/

  		//Vector<TaskAction*> taskLog;
  		VectorMap<TaskReference, Reference<TaskAction*> > lastTaskAction;

  		bool merging;

  	  public:
  		LocalTaskManager();

  		void initialize();

  		void start();

  		void shutdown();

  		void executeTask(Task* task);

#ifdef CXX11_COMPILER
		void executeTask(Function<void()>&& function, const char* name) {
			TaskManager::executeTask(std::move(function), name);
		}

		void executeTask(const Function<void()>& function, const char* name) {
			TaskManager::executeTask(function, name);
		}

		void scheduleTask(Function<void()>&& function, const char* name, uint64 delay) {
			TaskManager::scheduleTask(std::move(function), name, delay);
		}

		void scheduleTask(const Function<void()>& function, const char* name, uint64 delay) {
			TaskManager::scheduleTask(function, name, delay);
		}
#endif

  		void scheduleTask(Task* task, uint64 delay);
  		void scheduleTask(Task* task, const Time& time);

  		void rescheduleTask(Task* task, uint64 delay);
  		void rescheduleTask(Task* task, const Time& time);

  		bool cancelTask(Task* task);

  		Task* getTask();

  		bool isTaskScheduled(Task* task);

  		bool isTaskCancelled(Task* task);

		void mergeTasks(TaskManagerImpl* manager);

  		void flushTasks();

  		void undoTasks();

  		String getInfo(bool print);

  		int getScheduledTaskSize();

  		int getExecutingTaskSize();

  		bool getNextExecutionTime(Task* task, AtomicTime& nextExecutionTime);

  		bool isMerging() {
  			return merging;
  		}
  	  };

  } // namespace stm
} // namespace engine

using namespace engine::stm;

#endif /* LOCALTASKMANAGER_H_ */
