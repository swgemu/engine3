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
	Task* task = NULL;

	while ((task = taskManager->getTask()) != NULL) {
		try {
			while (true) {
				task->run();

				engine::stm::Transaction* transaction =
						engine::stm::Transaction::currentTransaction();

				if (transaction->commit())
					break;

				transaction->reset();
			}
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
