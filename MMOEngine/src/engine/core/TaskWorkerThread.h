/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TASKWORKERTHREAD_H_
#define TASKWORKERTHREAD_H_


#include "../service/ServiceThread.h"

namespace engine {
  namespace core {

	class TaskManager;

	class TaskWorkerThread : public ServiceThread {
		ReadWriteLock blockMutex;

	public:
		TaskWorkerThread(const String& s);

		virtual ~TaskWorkerThread();

		void start(TaskManager* manager);

		virtual void run();

		void stop();

		inline ReadWriteLock* getBlockMutex() {
			return &blockMutex;
		}
	};

  } // namespace service
} // namespace engine

using namespace engine::core;

#endif /* TASKWORKERTHREAD_H_ */
