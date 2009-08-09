/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "TaskQueue.h"
#include "Task.h"


TaskQueue::TaskQueue() : SortedVector<Task*>(200, 100), Condition(), Logger("TaskQueue") {
	blocked = false;
	waitingForTask = false;

	condMutex = new Mutex("TaskQueue");

	setLogging(false);
	//condMutex->setMutexLogging(false);
}

TaskQueue::~TaskQueue() {
	delete condMutex;
}

void TaskQueue::push(Task* task) {
	condMutex->lock();

	if (blocked) {
		condMutex->unlock();
		return;
	}

	SortedVector<Task*>::put(task);

	#ifdef TRACE_MESSAGES
		StringBuffer s;
		s << size() << " tasks in queue";
		info(s);
	#endif

	if (waitingForTask)
		signal(condMutex);

	condMutex->unlock();
}

Task* TaskQueue::pop() {
	condMutex->lock();

	#ifdef TRACE_MESSAGES
		info("waiting tasks");
	#endif

	while (isEmpty()) {
		if (blocked) {
			condMutex->unlock();
			return NULL;
		}

		waitingForTask = true;
		wait(condMutex);
	}

	Task* task = remove(0);
	task->setTaskScheduler(NULL);

	#ifdef TRACE_MESSAGES
		StringBuffer s;
		s << size() << " tasks remained in queue";
		info(s);
	#endif

	waitingForTask = false;
	condMutex->unlock();

	return task;
}

void TaskQueue::flush() {
	condMutex->lock();

	blocked = true;

	if (waitingForTask)
		signal(condMutex);

	condMutex->unlock();
}


