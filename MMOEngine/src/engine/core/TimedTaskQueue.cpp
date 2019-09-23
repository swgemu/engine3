/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include <pthread.h>
#include <errno.h>

#include "TimedTaskQueue.h"

TimedTaskQueue::TimedTaskQueue() : PriorityQueue(), Condition(), Logger("TaskQueue") {
	taskScheduler = nullptr;

	condMutex = new Mutex("TaskQueue");

	blocked = false;

	waitingForTask = false;
	changePlan = false;

	setLogging(false);
	setGlobalLogging(true);

	condMutex->setLockLogging(false);
}

TimedTaskQueue::~TimedTaskQueue() {
	delete condMutex;

	while (Task* task = static_cast<Task*>(poll())) {
		task->release();
	}
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

bool TimedTaskQueue::add(Task* task, const Time& time) {
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
	}

	if (task->isQueued())
		remove(task, false);

	#ifdef TRACE_TASKS
		info() << "adding task " << *task;
	#endif

	task->acquire();

	if (!task->setTaskScheduler(taskScheduler)) {
		task->release();

		condMutex->unlock(doLock);

		return true;
	}

	PriorityQueue::add(task);

	#ifdef TRACE_TASKS
		info() << "added task " << *task;
	#endif

	if (PriorityQueue::peak() == task && waitingForTask) {
		changePlan = true;
		signal(condMutex);
	}

	condMutex->unlock(doLock);

	return true;
}

bool TimedTaskQueue::addAll(PriorityQueue& queue, bool doLock) {
	condMutex->lock(doLock);

	if (blocked) {
		condMutex->unlock(doLock);
		return false;
	}

	auto task = queue.peak();

	PriorityQueue::merge(queue);

	if (PriorityQueue::peak() == task && waitingForTask) {
		changePlan = true;
		signal(condMutex);
	}

	condMutex->unlock(doLock);

	return true;
}

void TimedTaskQueue::wake() {
	condMutex->lock();

	signal(condMutex);

	condMutex->unlock();
}

Task* TimedTaskQueue::get() {
	condMutex->lock();

	waitingForTask = true;

	Time postWait;

	while (true) {
		if (PriorityQueue::isEmpty()) {
			if (blocked) {
				condMutex->unlock();
				return nullptr;
			}

			#ifdef TRACE_TASKS
				info("waiting task");
			#endif

			wait(condMutex);
		}

		if (PriorityQueue::isEmpty()) {
			condMutex->unlock();
			return nullptr;
		}

		const Task* task = static_cast<const Task*>(PriorityQueue::peak());
		Time time = task->getNextExecutionTime().getTimeObject();

		if (blocked || !time.isFuture())
			break;

		#ifdef TRACE_TASKS
			info() << "scheduling " << task->toStringData();
		#endif

		int res = timedWait(condMutex, &time);

		#ifdef TRACE_TASKS
			info() << "timedwait finished with (" << res << ")";
		#endif

#ifdef _MSC_VER
		if (res != 0 && res != ETIMEDOUT && res != 10060) {
#else
		if (res != 0 && res != ETIMEDOUT) {
#endif
			StringBuffer msg;

			if (res == 22)
				msg << "invalid condition parameters";
			else if (time.isFuture())
				msg << "misstiming (" << res << ") - future task happened" << task->getNextExecutionTime().getMiliTime();
			else
				msg << "condition error (" << res << ")";

			warning(msg);
			return nullptr;
		}

		postWait.updateToCurrentTime();

		if (changePlan) {
			#ifdef TRACE_TASKS
				info("changing plan");
			#endif

			changePlan = false;
		} else {
			if (time.compareTo(postWait) < 0) {
				int64 difference = -(postWait.getMiliTime() - task->getNextExecutionTime().getMiliTime());

				if (difference > 10) {
					condMutex->unlock();

					return nullptr;
				}
			}

			break;
		}
	}

	postWait.updateToCurrentTime();

	Task* task = static_cast<Task*>(PriorityQueue::poll());

	bool cleared = task->clearTaskScheduler();
	assert(cleared);

	if (!blocked && task->getNextExecutionTime().compareTo(postWait) < 0) {
		int64 difference = -(postWait.getMiliTime() - task->getNextExecutionTime().getMiliTime());

		if (difference > 10) {
			error() << "future task happenend " << task->getNextExecutionTime().getMiliTime() << " ("
				<< difference << ")";
		}
	} /*if (!blocked && e->getTimeStamp().miliDifference() > 250) {
		StringBuffer msg;
		msg << "WARNING too high delay " << e->getTimeStamp().getMiliTime() << " ("
			<< e->getTimeStamp().miliDifference() << ")";
		error(msg);
	}*/

	#ifdef TRACE_TASKS
		info() << "got task " << task->getNextExecutionTime().getMiliTime() << " [" << size() << "]";
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

			info() << "wrong taskScheduler when remove task->taskScheduler = " << hex << task->getTaskScheduler() << " this->taskScheduler " << hex << taskScheduler;
		#endif

		task->cancel();

		condMutex->unlock(doLock);

		return true;
	}

	const Task* next = static_cast<const Task*>(PriorityQueue::peak());

	PriorityQueue::remove(task);

	bool cleared = task->clearTaskScheduler();

	assert(cleared);

	task->release();

	#ifdef TRACE_TASKS
		info() << "removed task " << task->getNextExecutionTime().getMiliTime() << " [" << size() << "]";
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
