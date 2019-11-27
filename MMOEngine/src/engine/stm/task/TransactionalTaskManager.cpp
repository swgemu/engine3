/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/stm/TransactionalObjectHeader.h"

void TransactionalTaskManager::initialize() {
	taskManager = new TaskManagerImpl();
	taskManager->setLoggingName("TransactionalTaskmanager");
	taskManager->initialize(WORKER_THREADS, SCHEDULER_THREADS, 0);
	taskManager->setLogging(false);
	taskManager->setGlobalLogging(true);
}

void TransactionalTaskManager::start() {
	taskManager->start();
}

void TransactionalTaskManager::shutdown() {
	taskManager->shutdown();
}

#ifdef WITH_STM
void TransactionalTaskManager::retryTaskInSerial(Task* task) {
	taskManager->retryTaskInSerial(task);
}

Thread* TransactionalTaskManager::getSerialWorker() {
	return taskManager->getSerialWorker();
}
#endif

void TransactionalTaskManager::executeTask(Task* task) {
	LocalTaskManager* manager = getLocalTaskManager();

	manager->executeTask(task);
}

void TransactionalTaskManager::scheduleTask(Task* task, uint64 delay) {
	LocalTaskManager* manager = getLocalTaskManager();

	//manager->info("scheduling task");

	manager->scheduleTask(task, delay);
}

Vector<Pair<Locker*, TaskWorkerThread*>>* TransactionalTaskManager::blockTaskManager() {
	return taskManager->blockTaskManager();
}

void TransactionalTaskManager::unblockTaskManager(Vector<Pair<Locker*, TaskWorkerThread*>>* lockers) {
	taskManager->unblockTaskManager(lockers);
}

void TransactionalTaskManager::scheduleTask(Task* task, const Time& time) {
	LocalTaskManager* manager = getLocalTaskManager();

	//manager->info("scheduling task");

	manager->scheduleTask(task, time);
}

void TransactionalTaskManager::rescheduleTask(Task* task, uint64 delay) {
	LocalTaskManager* manager = getLocalTaskManager();

	manager->rescheduleTask(task, delay);
}

void TransactionalTaskManager::rescheduleTask(Task* task, const Time& time) {
	LocalTaskManager* impl = getLocalTaskManager();

	impl->rescheduleTask(task, time);
}

bool TransactionalTaskManager::cancelTask(Task* task) {
	LocalTaskManager* manager = getLocalTaskManager();

	//manager->info("cancelling task");

	return manager->cancelTask(task);
}

Task* TransactionalTaskManager::getTask() {
	return nullptr;
}

bool TransactionalTaskManager::isTaskScheduled(const Task* task) {
	LocalTaskManager* manager = getLocalTaskManager();

	if (!manager->isMerging())
		return manager->isTaskScheduled(task) || (taskManager->isTaskScheduled(task) && !manager->isTaskCancelled(task));
	else
		return taskManager->isTaskScheduled(task);
}

bool TransactionalTaskManager::getNextExecutionTime(const Task* task, AtomicTime& nextExecutionTime) {
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

String TransactionalTaskManager::getInfo(bool print) {
	return taskManager->getInfo(print);
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

	if (impl == nullptr) {
		impl = new LocalTaskManager();

		localTaskManager.set(impl);
	}

	return impl;
}
