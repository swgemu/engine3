#include "Core.h"

#include "TaskScheduler.h"

#include "Task.h"

#include "TaskManager.h"

Task::Task() : PriorityQueueEntry() {
	taskManager = Core::getTaskManager();

	priority = 3;

	reentrantTask = false;
}

Task::Task(uint64 mtime) : PriorityQueueEntry() {
	taskManager = Core::getTaskManager();

	nextExecutionTime.addMiliTime(mtime);

	priority = 3;

	reentrantTask = false;
}

Task::Task(Time& time) : PriorityQueueEntry() {
	taskManager = Core::getTaskManager();

	nextExecutionTime = time;

	priority = 3;

	reentrantTask = false;
}

Task::~Task() {
	if (isQueued()) {
		System::out << "ERROR: scheduled task deleted\n";
		raise(SIGSEGV);
	}
}

void Task::execute() {
#ifdef WITH_STM
	engine::stm::Transaction* transaction =	engine::stm::Transaction::currentTransaction();

	do {
		run();
	} while (!transaction->commit());

	delete transaction;
#else
	run();
#endif
}

bool Task::cancel() {
	return taskManager->cancelTask(this);
}

void Task::schedule(uint64 delay) {
	taskManager->scheduleTask(this, delay);
}

void Task::schedule(Time& time) {
	taskManager->scheduleTask(this, time);
}

void Task::reschedule(uint64 delay) {
	taskManager->rescheduleTask(this, delay);
}

void Task::reschedule(Time& time) {
	taskManager->rescheduleTask(this, time);
}

