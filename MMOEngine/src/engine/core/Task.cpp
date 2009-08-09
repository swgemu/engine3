#include "TaskScheduler.h"

#include "Task.h"

#include "TaskManager.h"

bool Task::cancel() {
	if (isQueued()) {
		//taskScheduler = TaskManager::instance()->getTaskScheduler();
		taskScheduler->cancelTask(this);

		return true;
	} else
		return false;
}

bool Task::reschedule(uint64 delay) {
	if (isQueued()) {
		//taskScheduler = TaskManager::instance()->getTaskScheduler();
		taskScheduler->rescheduleTask(this, delay);

		return true;
	} else
		return false;
}

bool Task::reschedule(Time& time) {
	if (isQueued()) {
		//taskScheduler = TaskManager::instance()->getTaskScheduler();
		taskScheduler->rescheduleTask(this, time);

		return true;
	} else
		return false;
}

