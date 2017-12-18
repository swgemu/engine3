/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "TaskQueue.h"
#include "Task.h"

TaskQueue::TaskQueue(const char* name) : Condition(), Logger("TaskQueue") {
	blocked = false;
	
	if (name != NULL)
		this->name = name;
	//waitingForTask = false;

	condMutex = new Mutex("TaskQueue");

	//setNoDuplicateInsertPlan();

	setLogging(false);
	//condMutex->setMutexLogging(false);
}

TaskQueue::~TaskQueue() {
	delete condMutex;
	condMutex = NULL;
}

void TaskQueue::pushRandom(Task* task) {
	condMutex->lock();

	if (blocked) {
		condMutex->unlock();
		return;
	}

	int size = LinkedList<Task*>::size();

	int position = size > 0 ? System::random(size - 1) : 0;

	LinkedList<Task*>::add(position, task);

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

void TaskQueue::wake() {
	condMutex->lock();

	signal(condMutex);

	condMutex->unlock();
}

void TaskQueue::pushFront(Task* task) {
	condMutex->lock();

	if (blocked) {
		condMutex->unlock();
		return;
	}

	LinkedList<Task*>::add(0, task);

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

void TaskQueue::push(Task* task) {
	condMutex->lock();

	if (blocked) {
		condMutex->unlock();
		return;
	}

	LinkedList<Task*>::add(task);

	task->acquire();

	#ifdef TRACE_TASKS
		StringBuffer s;
		s << size() << " tasks in queue";
		info(s);
	#endif

	//if (waitingForTask)
//		broadcast(condMutex);
	signal(condMutex);

	condMutex->unlock();

//	Thread::yield();
}

void TaskQueue::pushAll(const Vector<Task*>& tasks) {
	condMutex->lock();

	if (blocked) {
		condMutex->unlock();
		return;
	}

	for (int i = 0; i < tasks.size(); ++i) {
		LinkedList<Task*>::add(tasks.getUnsafe(i));
	}
	//Vector<Task*>::addAll(tasks);

	#ifdef TRACE_TASKS
		StringBuffer s;
		s << size() << " tasks in queue";
		info(s);
	#endif

	if (tasks.size() > 1)
		broadcast(condMutex);
	else
		signal(condMutex);

	condMutex->unlock();
}

Task* TaskQueue::pop() {
	condMutex->lock();

	#ifdef TRACE_TASKS
		info("waiting tasks");
	#endif

	/*while (isEmpty()) {
		if (blocked) {
			condMutex->unlock();
			return NULL;
		}

		//waitingForTask = true;
		wait(condMutex);
	}*/

	if (isEmpty()) {
		if (blocked) {
			condMutex->unlock();
			return NULL;
		}

		wait(condMutex);
	}

	if (isEmpty()) {
		condMutex->unlock();

		return NULL;
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

	while (!isEmpty()) {
		Task* task = remove(0);
		task->clearTaskScheduler();
		task->release();
	}

	condMutex->unlock();
}

int TaskQueue::size() {
	//condMutex->lock();

	int size = LinkedList<Task*>::size();

	//condMutex->unlock();

	return size;
}
