/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TASKQUEUE_H_
#define TASKQUEUE_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

//#define TRACE_TASKS

namespace engine {
  namespace core {

	class Task;

	class TaskQueue : public LinkedList<Task*>, public Condition, public Logger {
		Mutex* condMutex;

		bool blocked;
		//bool waitingForTask;

	public:
		TaskQueue();

		virtual ~TaskQueue();

		void pushFront(Task* task);
		void push(Task* task);
		void pushAll(const Vector<Task*>& tasks);
		void pushRandom(Task* task);

		Task* pop();

		void flush();

		int size();
	};

  } // namespace service
} // namespace engine

using namespace engine::core;

#endif /*TASKQUEUE_H_*/
