/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TASKWORKERTHREAD_H_
#define TASKWORKERTHREAD_H_


#include "engine/service/ServiceThread.h"

namespace engine {
  namespace core {

	class TaskManager;
	class TaskQueue;

	class TaskWorkerThread : public ServiceThread {
		Mutex blockMutex;
		TaskQueue* queue;

	public:
		TaskWorkerThread(const String& s, TaskQueue* queue);

		virtual ~TaskWorkerThread();

		void start(TaskManager* manager);

		//void pushToRetryQueue(Task* task);

		virtual void run();

		void stop();

		inline Mutex* getBlockMutex() {
			return &blockMutex;
		}
	};

  } // namespace service
} // namespace engine

using namespace engine::core;

#endif /* TASKWORKERTHREAD_H_ */
