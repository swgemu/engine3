/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "TaskQueue.h"
#include "Task.h"

TaskQueue::TaskQueue() : Vector<Task*>(200, 100), Condition(), Logger("TaskQueue") {
	blocked = false;
	//waitingForTask = false;

	condMutex = new Mutex("TaskQueue");

	//setNoDuplicateInsertPlan();

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

	if (Vector<Task*>::add(task))
		task->acquire();

	#ifdef TRACE_TASKS
		StringBuffer s;
		s << size() << " tasks in queue";
		info(s);
	#endif

	//if (waitingForTask)
		signal(condMutex);

	condMutex->unlock();
}

void TaskQueue::pushAll(const Vector<Task*>& tasks) {
	condMutex->lock();

	if (blocked) {
		condMutex->unlock();
		return;
	}

	Vector<Task*>::addAll(tasks);

	#ifdef TRACE_TASKS
		StringBuffer s;
		s << size() << " tasks in queue";
		info(s);
	#endif

	//if (waitingForTask)
		signal(condMutex);

	condMutex->unlock();
}

Task* TaskQueue::pop() {
	condMutex->lock();

	#ifdef TRACE_TASKS
		info("waiting tasks");
	#endif

	while (isEmpty()) {
		if (blocked) {
			condMutex->unlock();
			return NULL;
		}

		//waitingForTask = true;
		wait(condMutex);
	}

	Task* task = remove(0);
	task->setTaskScheduler(NULL);

	#ifdef TRACE_TASKS
		StringBuffer s;
		s << size() << " tasks remained in queue";
		info(s);
	#endif

	//waitingForTask = false;
	condMutex->unlock();

	return task;
}

void TaskQueue::flush() {
	condMutex->lock();

	blocked = true;

	//if (waitingForTask)
		broadcast(condMutex);

	condMutex->unlock();
}
