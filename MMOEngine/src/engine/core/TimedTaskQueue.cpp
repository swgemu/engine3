/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include <errno.h>

#include "TimedTaskQueue.h"

TimedTaskQueue::TimedTaskQueue() : PriorityQueue(), Condition(), Logger("TaskQueue") {
	taskScheduler = NULL;

	condMutex = new Mutex("TaskQueue");

	blocked = false;

	waitingForTask = false;
	changePlan = false;

	setLogging(true);
	condMutex->setLockLogging(false);
}

TimedTaskQueue::~TimedTaskQueue() {
	delete condMutex;
}

bool TimedTaskQueue::add(Task* task, uint64 delay) {
	condMutex->lock();

	if (task->isQueued())
		remove(task, false);

	if (delay != 0)
		task->updateExecutionTime(delay);

	bool result = add(task, false);

	condMutex->unlock();

	return result;
}

bool TimedTaskQueue::add(Task* task, Time& time) {
	condMutex->lock();

	if (task->isQueued())
		remove(task, false);

	task->setExecutionTime(time);

	bool result = add(task, false);

	condMutex->unlock();

	return result;
}

bool TimedTaskQueue::add(Task* task, bool doLock) {
	condMutex->lock(doLock);

	if (blocked) {
		condMutex->unlock(doLock);
		return false;
	} else if (task->getNextExecutionTime().isPast()) {
		error("past task scheduled - " + task->toStringData());

		/*StackTrace trace;
		trace.print();

		raise(SIGSEGV);*/

		condMutex->unlock(doLock);
		return false;
	}

	if (task->isQueued())
		remove(task, false);

	#ifdef TRACE_TASKS
		StringBuffer s1;
		s1 << "adding task " << task->toString();
		info(s1);
	#endif

	task->acquire();

	PriorityQueue::add(task);

	#ifdef TRACE_TASKS
		StringBuffer s;
		s << "added task " << task->toString();
		info(s);
	#endif

	if (PriorityQueue::peak() == task && waitingForTask) {
		changePlan = true;
		signal(condMutex);
	}

	condMutex->unlock(doLock);

	return true;
}

Task* TimedTaskQueue::get() {
	condMutex->lock();

	waitingForTask = true;

	while (true) {
		while (PriorityQueue::isEmpty()) {
			if (blocked) {
				condMutex->unlock();
				return NULL;
			}

			#ifdef TRACE_TASKS
				info("waiting task");
			#endif

			wait(condMutex);
		}

		Task* task = (Task*) PriorityQueue::peak();
		Time& time = task->getNextExecutionTime();

		if (blocked || !time.isFuture())
			break;

		#ifdef TRACE_TASKS
			StringBuffer s;
			s << "scheduling " << task->toString();
			info(s);
		#endif

		int res = timedWait(condMutex, &time);

		#ifdef TRACE_TASKS
			StringBuffer s2;
			s2 << "timedwait finished with (" << res << ")";
			info(s2);
		#endif

		if (res != 0 && res != ETIMEDOUT) {
			StringBuffer msg;

			if (res == 22)
				msg << "invalid condition parameters";
			else if (time.isFuture())
				msg << "misstiming (" << res << ") - future task happened" << task->getNextExecutionTime().getMiliTime();
			else
				msg << "condition error (" << res << ")";

			error(msg);
			continue;
		}

		if (changePlan) {
			#ifdef TRACE_TASKS
				info("changing plan");
			#endif

			changePlan = false;
		} else
			break;
	}

	Task* task = (Task*) PriorityQueue::poll();

	task->setTaskScheduler(taskScheduler);

	if (!blocked && task->getNextExecutionTime().isFuture()) {
		int64 difference = -(uint64) task->getNextExecutionTime().miliDifference();

		if (difference > 10) {
			StringBuffer msg;
			msg << "future task happenend " << task->getNextExecutionTime().getMiliTime() << " ("
				<< difference << ")";
			error(msg);
		}
	} /*if (!blocked && e->getTimeStamp().miliDifference() > 250) {
		StringBuffer msg;
		msg << "WARNING too high delay " << e->getTimeStamp().getMiliTime() << " ("
			<< e->getTimeStamp().miliDifference() << ")";
		error(msg);
	}*/

	#ifdef TRACE_TASKS
		StringBuffer s;
		s << "got task " << task->getNextExecutionTime().getMiliTime() << " [" << size() << "]";
		info(s);
	#endif

	waitingForTask = false;
	condMutex->unlock();

	return task;
}

bool TimedTaskQueue::remove(Task* task, bool doLock) {
	condMutex->lock(doLock);

	if (!task->isQueued()) {
		condMutex->unlock(doLock);

		return false;
	} else if (taskScheduler != task->getTaskScheduler()) {
		#ifdef TRACE_TASKS
			//info("wrong taskCheduler when removing task");

			StringBuffer msg;
			msg << "wrong taskScheduler when remove task->taskScheduler = " << hex << task->getTaskScheduler() << " this->taskScheduler " << hex << taskScheduler;
			info(msg.toString());
		#endif

		task->cancel();

		return true;
	}

	Task* next = (Task*) PriorityQueue::peak();

	PriorityQueue::remove(task);

	task->setTaskScheduler(NULL);

	task->release();

	#ifdef TRACE_TASKS
		StringBuffer s;
		s << "removed task " << task->getNextExecutionTime().getMiliTime() << " [" << size() << "]";
		info(s);
	#endif

	if (waitingForTask && next == task) {
		changePlan = true;
		signal(condMutex);
	}

	condMutex->unlock(doLock);

	return true;
}

void TimedTaskQueue::flush() {
	condMutex->lock();

	blocked = true;

	if (waitingForTask)
		broadcast(condMutex);

	condMutex->unlock();
}

void TimedTaskQueue::repair() {
	condMutex->lock();

	info("fixing queue");

	if (waitingForTask)
		signal(condMutex);

	condMutex->unlock();
}

void TimedTaskQueue::printQueue() {
	/*StringBuffer s;
	s << dec << "TaskQueue list [" << size() << "] = ";

	for (int i = 0; i < size(); ++i) {
		Task* ev = SortedVector<Task*>::get(i);
		s << ev->getTimeStamp().getMiliTime() << ", " ;
	}

	info(s);*/
}
