/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "TaskQueue.h"
#include "Task.h"

TaskQueue::TaskQueue(const char* name) : Condition(), Logger("TaskQueue") {
	blocked = false;

	if (name != nullptr)
		this->name = name;

	condMutex = new Mutex("TaskQueue");

	setLogging(false);
}

TaskQueue::~TaskQueue() {
	delete condMutex;
	condMutex = nullptr;
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
	info() << size() << " tasks in queue";
#endif

	signal(condMutex);

	condMutex->unlock();
}

void TaskQueue::wake() {
	condMutex->lock();

	broadcast(condMutex);

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
	info() << size() << " tasks in queue";
#endif

	signal(condMutex);

	condMutex->unlock();
}

void TaskQueue::push(Task* task) {
	condMutex->lock();

	if (blocked) {
		condMutex->unlock();
		return;
	}

	task->acquire();

	LinkedList<Task*>::add(task);

	#ifdef TRACE_TASKS
		info() << size() << " tasks in queue";
	#endif

	signal(condMutex);

	condMutex->unlock();
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

	#ifdef TRACE_TASKS
		info() << size() << " tasks in queue";
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

	if (isEmpty()) {
		if (blocked) {
			condMutex->unlock();
			return nullptr;
		}

		wait(condMutex);
	}

	if (isEmpty()) {
		condMutex->unlock();

		return nullptr;
	}

	Task* task = remove(0);
	task->setTaskScheduler(nullptr);

	#ifdef TRACE_TASKS
		info() << size() << " tasks remained in queue";
	#endif

	condMutex->unlock();

	return task;
}

void TaskQueue::flush() {
	condMutex->lock();

	blocked = true;

	broadcast(condMutex);

	while (!isEmpty()) {
		Task* task = remove(0);
		task->clearTaskScheduler();
		task->release();
	}

	condMutex->unlock();
}

void TaskQueue::waitToFinish() {
	while (!isEmpty()) {
		Thread::sleep(5);

		Locker locker(condMutex);

		signal(condMutex);
	}
}

int TaskQueue::size() const {
	//condMutex->lock();

	int size = LinkedList<Task*>::size();

	//condMutex->unlock();

	return size;
}
