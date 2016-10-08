/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/stm/TransactionalMemoryManager.h"

#include "TaskWorkerThread.h"

TaskWorkerThread::TaskWorkerThread(const String& s, TaskQueue* queue, int cpu) : ServiceThread(s) {
	setInfoLogLevel();
	setGlobalLogging(true);

	this->cpu = cpu;
	this->queue = queue;
}

TaskWorkerThread::~TaskWorkerThread() {
}

void TaskWorkerThread::start(TaskManager* manager) {
	taskManager = manager;

	debug("started");

	ServiceThread::start(false);
}

void TaskWorkerThread::run() {
	if (cpu) {
		assignToCPU(cpu);
	}

	Reference<Task*> task = NULL;

	while ((task = queue->pop()) != NULL) {
		blockMutex.lock();

		try {
//			debug("executing task");

			task->doExecute();
		} catch (Exception& e) {
			error(e.getMessage());
		} catch (...) {
			error("unreported Exception caught");
		}

		task->release();

		task = NULL;

		blockMutex.unlock();
	}
}

void TaskWorkerThread::stop() {
	ServiceThread::stop();

	info("stopped");
}
