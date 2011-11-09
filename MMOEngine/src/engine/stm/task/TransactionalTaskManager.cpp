/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/stm/TransactionalObjectHeader.h"
#include "engine/stm/Transaction.h"

#include "TransactionalTaskManager.h"

void TransactionalTaskManager::initialize() {
	taskManager = new TaskManagerImpl();
	taskManager->setLoggingName("TransactionalTaskmanager");
	taskManager->initialize(WORKER_THREADS, SCHEDULER_THREADS);
	taskManager->setLogging(false);
	taskManager->setGlobalLogging(true);
}

void TransactionalTaskManager::start() {
	taskManager->start();
}

void TransactionalTaskManager::shutdown() {
	taskManager->shutdown();
}

void TransactionalTaskManager::executeTask(Task* task) {
	LocalTaskManager* manager = getLocalTaskManager();

	manager->executeTask(task);
}

void TransactionalTaskManager::scheduleTask(Task* task, uint64 delay) {
	LocalTaskManager* manager = getLocalTaskManager();

	//manager->info("scheduling task");

	manager->scheduleTask(task, delay);
}

Vector<Locker*>* TransactionalTaskManager::blockTaskManager() {
	return taskManager->blockTaskManager();
}

void TransactionalTaskManager::unblockTaskManager(Vector<Locker*>* lockers) {
	taskManager->unblockTaskManager(lockers);
}

void TransactionalTaskManager::scheduleTask(Task* task, Time& time) {
	LocalTaskManager* manager = getLocalTaskManager();

	//manager->info("scheduling task");

	manager->scheduleTask(task, time);
}

void TransactionalTaskManager::rescheduleTask(Task* task, uint64 delay) {
	LocalTaskManager* manager = getLocalTaskManager();

	manager->rescheduleTask(task, delay);
}

void TransactionalTaskManager::rescheduleTask(Task* task, Time& time) {
	LocalTaskManager* impl = getLocalTaskManager();

	impl->rescheduleTask(task, time);
}

bool TransactionalTaskManager::cancelTask(Task* task) {
	LocalTaskManager* manager = getLocalTaskManager();

	//manager->info("cancelling task");

	return manager->cancelTask(task);
}

Task* TransactionalTaskManager::getTask() {
	return NULL;
}

bool TransactionalTaskManager::isTaskScheduled(Task* task) {
	LocalTaskManager* manager = getLocalTaskManager();

	if (!manager->isMerging())
		return manager->isTaskScheduled(task) || (taskManager->isTaskScheduled(task) && !manager->isTaskCancelled(task));
	else
		return taskManager->isTaskScheduled(task);
}

bool TransactionalTaskManager::getNextExecutionTime(Task* task, Time& nextExecutionTime) {
	LocalTaskManager* manager = getLocalTaskManager();

	//if (manager->......................................)
	bool res = manager->getNextExecutionTime(task, nextExecutionTime);

	if (!res) {
		nextExecutionTime = task->getNextExecutionTime();
	}

	return true;
}

void TransactionalTaskManager::flushTasks() {
	taskManager->flushTasks();
}

void TransactionalTaskManager::printInfo() {
	taskManager->printInfo();
}

int TransactionalTaskManager::getScheduledTaskSize() {
	return taskManager->getScheduledTaskSize();
}

int TransactionalTaskManager::getExecutingTaskSize() {
	return taskManager->getExecutingTaskSize();
}

void TransactionalTaskManager::execute() {
	LocalTaskManager* localTaskManager = getLocalTaskManager();

	localTaskManager->mergeTasks(taskManager);

	Task* task = Transaction::currentTransaction()->getTask();

	if (task->isPeriodic()) {
		taskManager->scheduleTask(task, task->getPeriod());

		//assert(task->isScheduled());
	}
}

void TransactionalTaskManager::undo() {
	LocalTaskManager* localTaskManager = getLocalTaskManager();

	localTaskManager->undoTasks();
}

LocalTaskManager* TransactionalTaskManager::getLocalTaskManager() {
	LocalTaskManager* impl = localTaskManager.get();

	if (impl == NULL) {
		impl = new LocalTaskManager();

		localTaskManager.set(impl);
	}

	return impl;
}
