/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TASKWORKERTHREAD_H_
#define TASKWORKERTHREAD_H_

#include "TaskManager.h"

#include "../service/ServiceThread.h"

namespace engine {
  namespace core {

	class TaskWorkerThread : public ServiceThread {
	public:
		TaskWorkerThread(const String& s);

		virtual ~TaskWorkerThread();

		void start(TaskManager* manager);

		virtual void run();

		void stop();
	};

  } // namespace service
} // namespace engine

using namespace engine::core;

#endif /* TASKWORKERTHREAD_H_ */
