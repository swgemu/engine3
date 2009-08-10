#include "TaskScheduler.h"

#include "Task.h"

#include "TaskManager.h"

bool Task::cancel() {
	return TaskManager::instance()->cancelTask(this);
}

void Task::schedule(uint64 delay) {
	TaskManager::instance()->scheduleTask(this, delay);
}

void Task::schedule(Time& time) {
	TaskManager::instance()->scheduleTask(this, time);
}

void Task::reschedule(uint64 delay) {
	TaskManager::instance()->rescheduleTask(this, delay);
}

void Task::reschedule(Time& time) {
	TaskManager::instance()->rescheduleTask(this, time);
}

