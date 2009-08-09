/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TASKQUEUE_H_
#define TASKQUEUE_H_

//#define TRACE_TASKS

#include "system/lang.h"

#include "../log/Logger.h"

namespace engine {
  namespace core {

	class Task;

	class TaskQueue : public SortedVector<Task*>, public Condition, public Logger {
		Mutex* condMutex;

		bool blocked;
		bool waitingForTask;

	public:
		TaskQueue();

		virtual ~TaskQueue();

		void push(Task* msg);

		Task* pop();

		void flush();
	};

  } // namespace service
} // namespace engine

using namespace engine::core;

#endif /*TASKQUEUE_H_*/
