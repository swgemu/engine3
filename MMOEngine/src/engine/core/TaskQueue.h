/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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
		void waitToFinish();

		int size() const;

		const String& getName() const {
			return name;
		}
	};

  } // namespace service
} // namespace engine

using namespace engine::core;

#endif /*TASKQUEUE_H_*/
