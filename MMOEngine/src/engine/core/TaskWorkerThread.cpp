/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/stm/TransactionalMemoryManager.h"
#include "engine/db/ObjectDatabaseManager.h"

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
		#ifdef WITH_STM
			engine::stm::Transaction* transaction =	engine::stm::Transaction::currentTransaction();

			do {
				task->run();
			} while (!transaction->commit());

			delete transaction;
		#else
			task->run();
		#endif
		} catch (Exception& e) {
			error(e.getMessage());
		} catch (...) {
			error("unreported Exception caught");
		}

		ObjectDatabaseManager::instance()->commitLocalTransaction();

		task->release();
	}
}

void TaskWorkerThread::stop() {
	ServiceThread::stop();

	info("stopped");
}
