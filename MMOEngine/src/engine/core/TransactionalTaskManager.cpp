/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "TransactionalTaskManager.h"

void TransactionalTaskManager::initialize() {
	taskManager = new TaskManagerImpl();
	taskManager->initialize(1, 1);
}

void TransactionalTaskManager::start() {
	taskManager->start();
}

void TransactionalTaskManager::shutdown() {
	taskManager->shutdown();
}

void TransactionalTaskManager::executeTask(Task* task) {
	TaskManagerImpl* manager = getLocalTaskManager();

	manager->executeTask(task);
}

void TransactionalTaskManager::scheduleTask(Task* task, uint64 delay) {
	TaskManagerImpl* manager = getLocalTaskManager();

	manager->scheduleTask(task, delay);
}

void TransactionalTaskManager::scheduleTask(Task* task, Time& time) {
	TaskManagerImpl* manager = getLocalTaskManager();

	manager->scheduleTask(task, time);
}

void TransactionalTaskManager::rescheduleTask(Task* task, uint64 delay) {
	TaskManagerImpl* manager = getLocalTaskManager();

	manager->rescheduleTask(task, delay);
}

void TransactionalTaskManager::rescheduleTask(Task* task, Time& time) {
	TaskManagerImpl* impl = getLocalTaskManager();

	impl->rescheduleTask(task, time);
}

bool TransactionalTaskManager::cancelTask(Task* task) {
	TaskManagerImpl* manager = getLocalTaskManager();

	return manager->cancelTask(task);
}

Task* TransactionalTaskManager::getTask() {
	return NULL;
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
	TaskManagerImpl* localTaskManager = getLocalTaskManager();

	taskManager->mergeTasks(localTaskManager);

	localTaskManager->clearTasks();
}

void TransactionalTaskManager::undo() {
	TaskManagerImpl* localTaskManager = getLocalTaskManager();

	localTaskManager->clearTasks();
}

TaskManagerImpl* TransactionalTaskManager::getLocalTaskManager() {
	TaskManagerImpl* impl = localTaskManager.get();

	if (impl == NULL) {
		impl = new TaskManagerImpl();
		impl->initialize(0, 1);
	}

	return impl;
}
