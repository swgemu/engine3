/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * qtstmtest.h
 *
 *  Created on: 11/11/2011
 *      Author: victor
 */

#ifndef QTSTMTEST_H_
#define QTSTMTEST_H_

#include "engine/engine.h"

#define MINCOORD -512
#define MAXCOORD 512

//#define RANDOM_MOVEMENT

//#define START_RANDOM_POSITION

#ifdef WITH_STM
#ifdef RANDOM_MOVEMENT
#define TASKSTOQUEUE 250000
#else
#define TASKSTOQUEUE 30000
#endif
#else
#ifdef RANDOM_MOVEMENT
#define TASKSTOQUEUE 900000
#else
#define TASKSTOQUEUE 600000
#endif
#endif

#define OBJECTCOUNT 300



Mutex qtMutex;

AtomicInteger taskCount;

#ifndef WITH_STM
class QTStatsTask : public Task {
public:
	void run() {
		char str[50];
		sprintf(str, "[QTStatsTask] %d tasks executed", taskCount.get());
		taskCount = 0;

		Logger::console.info(str, true);

		schedule(1000);
	}
};
#endif

class QTMoveTask : public Task {
	//TransactionalReference<TestClass*> >* references;
	Reference<QuadTreeEntry*> entry;
	TransactionalReference<QuadTree*> qt;

public:
	QTMoveTask(Reference<QuadTreeEntry*>& entr, TransactionalReference<QuadTree*>& quad) {
		entry = entr;
		qt = quad;
	}

	void run() {
#ifdef WITH_STM
		QuadTree* quadTree = qt.get();

#ifdef RANDOM_MOVEMENT
		entry->setPosition(System::random(MAXCOORD * 2) + MINCOORD, 0, System::random(MAXCOORD * 2) + MINCOORD);
#else
		entry->setPosition(0, 0, 0);
#endif
		quadTree->update(entry);
		quadTree->inRange(entry, 512);
#else
		Locker locker(entry);

		Locker locker2(&qtMutex);

#ifdef RANDOM_MOVEMENT
		entry->setPosition(System::random(MAXCOORD * 2) + MINCOORD, 0, System::random(MAXCOORD * 2) + MINCOORD);
#else
		entry->setPosition(0, 0, 0);
#endif

		qt->update(entry);
		qt->inRange(entry, 512);

		taskCount.increment();
#endif
	}
};

void testQTSTM() {
#ifdef WITH_STM
	Reference<Transaction*> transaction = TransactionalMemoryManager::instance()->startTransaction();
#endif

	TransactionalReference<QuadTree*> qt = new QuadTree(MINCOORD, MINCOORD, MAXCOORD, MAXCOORD);
	Vector<Reference<QuadTreeEntry*> > objects;

#ifdef WITH_STM
	printf("Starting STM test with %d workers\n", TransactionalTaskManager::WORKER_THREADS);
#else
	printf("Starting LOCK test with %d workers\n", TaskManagerImpl::DEFAULT_WORKER_THREADS);
#endif

	printf("Creating %d objects..\n", OBJECTCOUNT);

	int j = 0;

	for (int i = 0; i < OBJECTCOUNT; ++i) {
		Reference<QuadTreeEntry*> entry = new QuadTreeEntry();

		entry->_setObjectID(i);

#ifdef START_RANDOM_POSITION
		entry->initializePosition(System::random(MAXCOORD * 2) + MINCOORD, 0, System::random(MAXCOORD * 2) + MINCOORD);
#else
		entry->initializePosition(0, 0, 0);
#endif
		objects.add(entry);
	}

	printf("Inserting objects into QuadTree...\n");

	for (int i = 0; i < objects.size(); ++i) {
		Reference<QuadTreeEntry*>& obj = objects.get(i);

		qt->insert(obj);

		qt->inRange(obj, 512);
	}

	int maxInRangeObjects = 0;

	/*for (int i = 0; i < objects.size(); ++i) {
		Reference<QuadTreeEntry*>& obj = objects.get(i);

		if (obj->inRangeObjectCount() > maxInRangeObjects)
			maxInRangeObjects = obj->inRangeObjectCount();
	}

	printf("maxInRangeObjects = %d\n", maxInRangeObjects);*/

#ifdef WITH_STM
	printf("Commiting pure transaction...\n");

	TransactionalMemoryManager::commitPureTransaction(transaction);
#endif
	int totalTasks = 0;

	int taskToExecute = TASKSTOQUEUE;

#ifndef WITH_STM
	printf("queuing stats task\n");

	Task* task = new QTStatsTask();
	task->schedule(1000);
#endif

	printf("starting tasks\n");

	Time start;

#ifdef WITH_STM
		transaction = TransactionalMemoryManager::instance()->startTransaction();
#endif
		for (int i = 0; i < taskToExecute; ++i) {
			Reference<Task*> task = new QTMoveTask(objects.get(System::random(objects.size() - 1)), qt);

			++totalTasks;

			task->execute();
		}

#ifdef WITH_STM
		TransactionalMemoryManager::commitPureTransaction(transaction);
#endif

	printf("queued %d tasks\n", totalTasks);

	int executedTasks = Core::getTaskManager()->getExecutingTaskSize();

	printf("waiting to finsih...");

	Time delay;

	while (executedTasks != 0) {
		executedTasks = Core::getTaskManager()->getExecutingTaskSize();
		Thread::sleep(10);
	}

	printf("finished in %lld\n", start.miliDifference());

	printf("total AtomicInteger::increment %lld total AtomicInteger::decrement %lld\n", AtomicInteger::totalIncrementCount, AtomicInteger::totalDecrementCount);

	maxInRangeObjects = 0;

#ifdef WITH_STM
	transaction = TransactionalMemoryManager::instance()->startTransaction();
#endif

	/*for (int i = 0; i < objects.size(); ++i) {
		Reference<QuadTreeEntry*>& obj = objects.get(i);

		if (obj->inRangeObjectCount() > maxInRangeObjects)
			maxInRangeObjects = obj->inRangeObjectCount();
	}

	printf("maxInRangeObjects after updates = %d\n", maxInRangeObjects);*/

#ifdef WITH_STM
	TransactionalMemoryManager::commitPureTransaction(transaction);

	//printf("QuadTreeEntryImplementation clone max time %lld\n", ObjectCloner<QuadTreeEntryImplementation>::maxTime);
	//printf("CLone count %d\n", CloneCounter::cloneCount.get());

	printf("ArrayClone count:%d\n", ArrayCloneCounter::totalCount);
#endif

	printf("\n");

#ifdef WITH_STM
	TransactionalMemoryManager::instance()->printStatistics();
#endif

	exit(0);
}


#endif /* QTSTMTEST_H_ */
