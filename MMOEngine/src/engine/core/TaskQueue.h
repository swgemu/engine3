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
		String name;

	public:
		TaskQueue(const char* name = nullptr);

		virtual ~TaskQueue();

		void pushFront(Task* task);
		void push(Task* task);
		void pushAll(const Vector<Task*>& tasks);
		void pushRandom(Task* task);

		Task* pop();

		void wake();

		void flush();

		int size();

		const String& getName() const {
			return name;
		}
	};

  } // namespace service
} // namespace engine

using namespace engine::core;

#endif /*TASKQUEUE_H_*/
