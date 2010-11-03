/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/stm/TransactionalMemoryManager.h"

#include "TaskWorkerThread.h"

#include "TaskManager.h"

#include "Task.h"

TaskWorkerThread::TaskWorkerThread(const String& s) : ServiceThread(s) {
	setLogging(false);
}

TaskWorkerThread::~TaskWorkerThread() {
}

void TaskWorkerThread::start(TaskManager* manager) {
	taskManager = manager;

	info("started");

	ServiceThread::start(false);
}

void TaskWorkerThread::run() {
	Reference<Task*> task = NULL;

	while ((task = taskManager->getTask()) != NULL) {
		try {
			task->execute();
		} catch (Exception& e) {
			error(e.getMessage());
		} catch (...) {
			error("unreported Exception caught");
		}

		task->release();
	}
}

void TaskWorkerThread::stop() {
	ServiceThread::stop();

	info("stopped");
}
