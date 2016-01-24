/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/db/ObjectDatabaseManager.h"

#include "TaskWorkerThread.h"

#include "TaskManagerImpl.h"

#include "TaskScheduler.h"

#include "Task.h"

TaskManagerImpl::TaskManagerImpl() : Mutex("TaskManager"), Logger("TaskManager") {
	shuttingDown = false;

#ifdef WITH_STM
	serialWorker = NULL;
#endif

	setInfoLogLevel();
}

TaskManagerImpl::~TaskManagerImpl() {
}

void TaskManagerImpl::initialize() {
	initialize(DEFAULT_WORKER_THREADS, DEFAULT_SCHEDULER_THREADS, DEFAULT_IO_SCHEDULER_THREADS);
}

void TaskManagerImpl::initialize(int workerCount, int schedulerCount, int ioCount) {
	ObjectDatabaseManager::instance()->setLogLevel(getLogLevel());

	Locker locker(this);

	for (int j = 0; j < DEFAULT_TASK_QUEUES; ++j) {
		TaskQueue* queue = new TaskQueue();
		queue->setLogLevel(getLogLevel());
		taskQueues.add(queue);
		
		for (int i = 0; i < workerCount; ++i) {
			TaskWorkerThread* worker = new TaskWorkerThread("TaskWorkerThread" + String::valueOf(i), queue);
			worker->setLogLevel(getLogLevel());
			workers.add(worker);
		}
		
		
	}

#ifdef WITH_STM
	serialWorker = new TaskWorkerThread("TaskSerialWorkerThread", &serialTaskQueue);

	workers.add(serialWorker);
#endif

	for (int i = 0; i < schedulerCount; ++i) {
		TaskScheduler* scheduler = new TaskScheduler("TaskScheduler" + String::valueOf(i));
		scheduler->setTaskManager(this);
		scheduler->setLogLevel(getLogLevel());

		schedulers.add(scheduler);
	}

	for (int i = 0; i < ioCount; ++i) {
		TaskScheduler* scheduler = new TaskScheduler("IoTaskScheduler" + String::valueOf(i));
		scheduler->setTaskManager(this);
		scheduler->setLogLevel(getLogLevel());

		ioSchedulers.add(scheduler);
	}
	
	StringBuffer msg;
	msg << "initialized";
	debug(msg);
}

void TaskManagerImpl::start() {
	Locker locker(this);

	for (int i = 0; i < workers.size(); ++i) {
		TaskWorkerThread* worker = workers.get(i);
		worker->start(this);
	}

	for (int i = 0; i < schedulers.size(); ++i) {
		TaskScheduler* scheduler = schedulers.get(i);
		scheduler->start();
	}

	for (int i = 0; i < ioSchedulers.size(); ++i) {
		TaskScheduler* scheduler = ioSchedulers.get(i);
		scheduler->start();
	}

	StringBuffer msg;
	msg << "started";
	debug(msg);
}

void TaskManagerImpl::setLogLevel(int level) {
	Logger::setLogLevel(level);
}

void TaskManagerImpl::shutdown() {
	shuttingDown = true;

	while (!schedulers.isEmpty()) {
		lock();

		Reference<TaskScheduler*> scheduler = schedulers.remove(0);

		unlock();

		scheduler->stop();
	}

	while (!ioSchedulers.isEmpty()) {
		lock();

		Reference<TaskScheduler*> scheduler = ioSchedulers.remove(0);

		unlock();

		scheduler->stop();
	}

	while (!taskQueues.isEmpty()) {
		lock();

		TaskQueue* taskQueue = taskQueues.remove(0);
		taskQueue->flush();

		//delete taskQueue;

		unlock();

	}
	/*
	for(int i = 0; i < taskQueues.size(); ++i) {
		taskQueues.get(i)->flush();
	}
	*/
	while (!workers.isEmpty()) {
		lock();

		Reference<TaskWorkerThread*> worker = workers.remove(0);

		unlock();

		worker->stop();
	}

	debug("stopped");
}

Vector<Locker*>* TaskManagerImpl::blockTaskManager() {
	//Locker locker(this);

	Vector<Locker*>* lockers = new Vector<Locker*>();

	for (int i = 0; i < workers.size(); ++i) {
//		if (i == 9 || i == 7) //mysql and bas packet handler workers should continue
//			continue;
			
		TaskWorkerThread* worker = workers.get(i);

		Mutex* blockMutex = worker->getBlockMutex();

		Locker* locker = new Locker(blockMutex);
		lockers->add(locker);
	}

	for (int i = 0; i < schedulers.size(); ++i) {
		TaskScheduler* scheduler = schedulers.get(i);

		Mutex* blockMutex = scheduler->getBlockMutex();

		Locker* locker = new Locker(blockMutex);
		lockers->add(locker);
	}

/* dont block base clients for db update
	for (int i = 0; i < ioSchedulers.size(); ++i) {
		TaskScheduler* scheduler = ioSchedulers.get(i);

		Mutex* blockMutex = scheduler->getBlockMutex();

		Locker* locker = new Locker(blockMutex);
		lockers->add(locker);
	}
	*/

	return lockers;
}

void TaskManagerImpl::unblockTaskManager(Vector<Locker*>* lockers) {
	for (int i = 0; i < lockers->size(); ++i)
		delete lockers->get(i);

	lockers->removeAll();
}

TaskScheduler* TaskManagerImpl::getIoTaskScheduler() {
	//Locker locker(this);

	if (ioSchedulers.isEmpty()) {
		if (!shuttingDown)
			throw Exception("No schedulers available");
		else
			return NULL;
	}

	int index = currentTaskScheduler.increment() % ioSchedulers.size();

	return ioSchedulers.get(index);
}

TaskScheduler* TaskManagerImpl::getTaskScheduler() {
	//Locker locker(this); no need for this all is atomic

	if (schedulers.isEmpty()) {
		if (!shuttingDown)
			throw Exception("No schedulers available");
		else
			return NULL;
	}

	int index = currentTaskScheduler.increment() % schedulers.size();

	return schedulers.get(index);
}

void TaskManagerImpl::setTaskScheduler(Task* task, TaskScheduler* scheduler) {
	if (!task->setTaskScheduler(scheduler))
		throw IllegalArgumentException("task is already scheduled");
}

void TaskManagerImpl::executeTask(Task* task) {
	taskQueues.get(0)->push(task);
}

void TaskManagerImpl::executeTask(Task* task, int taskqueue) {
	taskQueues.get(taskqueue)->push(task);
}

#ifdef CXX11_COMPILER
void TaskManagerImpl::executeTask(std::function<void()>&& function) {
	class LambdaTask : public Task {
		std::function<void()> lambda;

	public:
		LambdaTask(std::function<void()>&& lam) : lambda(lam) {
		}

		void run() {
			lambda();
		}
	};

	LambdaTask* lambdaTask = new LambdaTask(std::move(function));
	lambdaTask->execute();
}

void TaskManagerImpl::executeTask(const std::function<void()>& function) {
	class LambdaTask : public Task {
		std::function<void()> lambda;

	public:
		LambdaTask(const std::function<void()>& lam) : lambda(lam) {
		}

		void run() {
			lambda();
		}
	};

	LambdaTask* lambdaTask = new LambdaTask(function);
	lambdaTask->execute();
}

void TaskManagerImpl::scheduleTask(std::function<void()>&& function, uint64 delay) {
	class LambdaTask : public Task {
		std::function<void()> lambda;

	public:
		LambdaTask(std::function<void()>&& lam) : lambda(lam) {
		}

		void run() {
			lambda();
		}
	};

	LambdaTask* lambdaTask = new LambdaTask(std::move(function));
	lambdaTask->schedule(delay);
}

void TaskManagerImpl::scheduleTask(const std::function<void()>& function, uint64 delay) {
	class LambdaTask : public Task {
		std::function<void()> lambda;

	public:
		LambdaTask(const std::function<void()>& lam) : lambda(lam) {
		}

		void run() {
			lambda();
		}
	};

	LambdaTask* lambdaTask = new LambdaTask(function);
	lambdaTask->schedule(delay);
}
#endif

void TaskManagerImpl::executeTaskFront(Task* task) {
	taskQueues.get(0)->pushFront(task);
}

void TaskManagerImpl::executeTaskRandom(Task* task) {
	taskQueues.get(0)->pushRandom(task);
}

void TaskManagerImpl::executeTasks(const Vector<Task*>& taskList) {
	taskQueues.get(0)->pushAll(taskList);
}

bool TaskManagerImpl::getNextExecutionTime(Task* task, Time& nextExecutionTime) {
	nextExecutionTime = task->getNextExecutionTime();

	return true;
}

bool TaskManagerImpl::isTaskScheduled(Task* task) {
	return task->getTaskScheduler() != NULL;
}

void TaskManagerImpl::scheduleIoTask(Task* task, uint64 delay) {
	Locker locker(this);

	if (task->isScheduled()) {
#ifdef TRACE_TASKS
		task->printScheduleTrace();
#endif

		throw IllegalArgumentException("task is already scheduled");
	}

	TaskScheduler* scheduler = getIoTaskScheduler();
	if (scheduler == NULL)
		return;

	locker.release();

	if (!scheduler->scheduleTask(task, delay))
		throw IllegalArgumentException("Task was invalid for scheduling");
}

void TaskManagerImpl::scheduleIoTask(Task* task, Time& time) {
	Locker locker(this);

	if (task->isScheduled()) {
#ifdef TRACE_TASKS
		task->printScheduleTrace();
#endif

		throw IllegalArgumentException("task is already scheduled");
	}

	TaskScheduler* scheduler = getIoTaskScheduler();
	if (scheduler == NULL)
		return;

	locker.release();

	if (!scheduler->scheduleTask(task, time))
		throw IllegalArgumentException("Task was invalid for scheduling");
}

void TaskManagerImpl::scheduleTask(Task* task, uint64 delay) {
	Locker locker(this);

	if (task->isScheduled()) {
	#ifdef TRACE_TASKS
		task->printScheduleTrace();
	#endif

		throw IllegalArgumentException("task is already scheduled");
	}

	TaskScheduler* scheduler = getTaskScheduler();
	if (scheduler == NULL)
		return;

	locker.release();

	if (!scheduler->scheduleTask(task, delay))
		throw IllegalArgumentException("Task was invalid for scheduling");
}

void TaskManagerImpl::scheduleTask(Task* task, Time& time) {
	Locker locker(this);

	if (task->isScheduled()) {
	#ifdef TRACE_TASKS
		task->printScheduleTrace();
	#endif

		throw IllegalArgumentException("task is already scheduled");
	}

	TaskScheduler* scheduler = getTaskScheduler();
	if (scheduler == NULL)
		return;

	locker.release();

	if (!scheduler->scheduleTask(task, time))
		throw IllegalArgumentException("Task was invalid for scheduling");
}

void TaskManagerImpl::rescheduleIoTask(Task* task, uint64 delay) {
	task->acquire();

	cancelTask(task);

	scheduleIoTask(task, delay);

	task->release();
}

void TaskManagerImpl::rescheduleIoTask(Task* task, Time& time) {
	task->acquire();

	cancelTask(task);

	scheduleIoTask(task, time);

	task->release();
}

void TaskManagerImpl::rescheduleTask(Task* task, uint64 delay) {
	task->acquire();

	cancelTask(task);

	scheduleTask(task, delay);

	task->release();
}

void TaskManagerImpl::rescheduleTask(Task* task, Time& time) {
	task->acquire();

	cancelTask(task);

	scheduleTask(task, time);

	task->release();
}

bool TaskManagerImpl::cancelTask(Task* task) {
	Locker locker(this);

	if (!task->isScheduled())
		return false;

	TaskScheduler* scheduler = task->getTaskScheduler();
	//assert (scheduler != NULL);

	locker.release();

	if (scheduler == NULL)
		return false;

	return scheduler->cancelTask(task);
}

void TaskManagerImpl::flushTasks() {
	Locker locker(this);

	//tasks.flush();
	
	for (int i = 0; i < taskQueues.size(); ++i) {
		taskQueues.get(i)->flush();
	}

	for (int i = 0; i < schedulers.size(); ++i) {
		TaskScheduler* scheduler = schedulers.get(i);

		scheduler->flushTasks();
	}

}

String TaskManagerImpl::getInfo(bool print) {
	lock();

	StringBuffer msg;
//	msg << "executing tasks - " << getExecutingTaskSize();

	for (int i = 0; i < taskQueues.size(); ++i) {
		msg << "executing tasks in queue" << i << " " << taskQueues.get(i)->size() << endl;
	}

	if (print)
		info(msg);

	StringBuffer msg2;
	msg2 << "scheduled tasks - " << getScheduledTaskSize() << endl;

	for (int i = 0; i < schedulers.size(); ++i) {
		TaskScheduler* scheduler = schedulers.get(i);

		HashTable<const char*, uint64> tasksCount = scheduler->getTasksCount();

		//lets order them
		VectorMap<uint64, const char*> ordered;

		HashTableIterator<const char*, uint64> iterator = tasksCount.iterator();

		while (iterator.hasNext()) {
			const char* name;
			uint64 count;

			iterator.getNextKeyAndValue(name, count);

			ordered.put(count, name);
		}

		msg2 << "scheduled tasks in scheduler " << i << " - " << scheduler->getQueueSize();
		msg2 << " pushed - " << scheduler->getPushedTasks() << " popped - " << scheduler->getPoppedTasks() << " removed - " << scheduler->getRemovedTasks() << endl;

		//lets print top 5
		for (int i = 0, j = ordered.size() - 1; i < 5 && (j - i) >= 0; ++i) {
			int index = j - i;
			msg2 << ordered.elementAt(index).getValue() << ":" << ordered.elementAt(index).getKey() << endl;
		}
	}

	if (print)
		info(msg2);

	StringBuffer msg3;
	msg3 << "io scheduled tasks - " << getIoScheduledTaskSize();

	if (print)
		info(msg3);

	unlock();

	msg << endl << msg2.toString() << endl << msg3.toString();
	return msg.toString();
}

class TestTask : public Task {
	int value;

public:
	TestTask(int val) {
		value = val;
	}

	void run() {
		System::out.println("test" + String::valueOf(value));
	}
};

void TaskManagerImpl::testScheduler() {
	for (int i = 0; i < 20; ++i) {
		uint32 shift = System::random(100);

		Task* task = new TestTask(i);

		scheduleTask(task, 100 + shift);
		rescheduleTask(task, shift - 50);

		Thread::sleep(System::random(5) + 5);
	}

	/*Task* task = new ReentrantTestTask(1);
	scheduleTask(task, 1000);*/
}

int TaskManagerImpl::getScheduledTaskSize() {
	int sum = 0;

	for (int i = 0; i < schedulers.size(); ++i) {
		TaskScheduler* scheduler = schedulers.get(i);

		sum += scheduler->getQueueSize();
	}

	return sum;
}

int TaskManagerImpl::getIoScheduledTaskSize() {
	int sum = 0;

	for (int i = 0; i < ioSchedulers.size(); ++i) {
		TaskScheduler* scheduler = ioSchedulers.get(i);

		sum += scheduler->getQueueSize();
	}

	return sum;
}

int TaskManagerImpl::getExecutingTaskSize() {
//	return tasks.size();

	int count = 0;
	for (int i = 0; i < taskQueues.size(); ++i) {
		count += taskQueues.get(i)->size();
	}

#ifdef WITH_STM
	count += serialTaskQueue.size();
#endif
	
	return count;
}

#ifdef WITH_STM
void TaskManagerImpl::retryTaskInSerial(Task* task) {
	//serialWorker->pushToRetryQueue(task);
	serialTaskQueue.push(task);
}

Thread* TaskManagerImpl::getSerialWorker() {
	return serialWorker;
}
#endif
