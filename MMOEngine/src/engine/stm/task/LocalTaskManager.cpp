/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "LocalTaskManager.h"

void LocalTaskManager::initialize() {
}

void LocalTaskManager::start() {
}

void LocalTaskManager::shutdown() {
}

void LocalTaskManager::executeTask(Task* task) {
	if (executedTasks.put(task) != -1) {
		task->acquire();
	}
}

void LocalTaskManager::scheduleTask(Task* task, uint64 delay) {
	if (task->isScheduled())
		throw IllegalArgumentException("task already scheduled");

	if (scheduledTasks.put(task) != -1) {
		task->acquire();

		if (delay != 0)
			task->updateExecutionTime(delay);
	}
}

void LocalTaskManager::scheduleTask(Task* task, Time& time) {
	if (task->isScheduled())
		throw IllegalArgumentException("task already scheduled");

	if (scheduledTasks.put(task) != -1) {
		task->acquire();

		task->setExecutionTime(time);
	}
}

void LocalTaskManager::rescheduleTask(Task* task, uint64 delay) {
	cancelTask(task);

	scheduleTask(task, delay);
}

void LocalTaskManager::rescheduleTask(Task* task, Time& time) {
	cancelTask(task);

	scheduleTask(task, time);
}

bool LocalTaskManager::cancelTask(Task* task) {
	if (!task->isScheduled())
		return false;

	if (scheduledTasks.removeElement(task)) {
		task->release();
	}

	return cancelledTasks.put(task) != -1;
}

Task* LocalTaskManager::getTask() {
	return NULL;
}

bool LocalTaskManager::isTaskScheduled(Task* task) {
	return scheduledTasks.contains(task);
}

bool LocalTaskManager::isTaskCancelled(Task* task) {
	return cancelledTasks.contains(task);
}

void LocalTaskManager::mergeTasks(TaskManagerImpl* manager) {
	manager->info("executing " + String::valueOf(executedTasks.size()) +
			", adding " + String::valueOf(scheduledTasks.size()) +
			", canceling " + String::valueOf(cancelledTasks.size()) +" tasks");

	manager->executeTasks(executedTasks);

	for (int i = 0; i < cancelledTasks.size(); ++i) {
		Task* task = cancelledTasks.get(i);

		manager->cancelTask(task);
	}

	cancelledTasks.removeAll();

	Vector<Task*> tasks = scheduledTasks;

	scheduledTasks.removeAll();

	for (int i = 0; i < tasks.size(); ++i) {
		Task* task = tasks.get(i);

		try {
			manager->scheduleTask(task, task->getNextExecutionTime());
		} catch (IllegalArgumentException& e) {
			manager->error(e.getMessage() + " for " + task->toStringData());
		}

		task->release();
	}

	flushTasks();
}

void LocalTaskManager::flushTasks() {
	executedTasks.removeAll();

	scheduledTasks.removeAll();
	cancelledTasks.removeAll();
}

void LocalTaskManager::printInfo() {
}

int LocalTaskManager::getScheduledTaskSize() {
	return scheduledTasks.size();
}

int LocalTaskManager::getExecutingTaskSize() {
	return executedTasks.size();
}
