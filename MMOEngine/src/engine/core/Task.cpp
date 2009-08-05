#include "TaskScheduler.h"

#include "Task.h"

bool Task::cancel() {
	if (isQueued()) {
		taskScheduler->cancelTask(this);

		return true;
	} else
		return false;
}

bool Task::reschedule(uint64 delay) {
	if (isQueued()) {
		taskScheduler->rescheduleTask(this, delay);

		return true;
	} else
		return false;
}

bool Task::reschedule(Time& time) {
	if (isQueued()) {
		taskScheduler->rescheduleTask(this, time);

		return true;
	} else
		return false;
}

