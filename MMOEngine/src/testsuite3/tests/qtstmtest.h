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
#define ITERATIONS 60
#define OBJECTCOUNT 300

Mutex qtMutex;

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
		QuadTree* quadTree = qt.getForUpdate();

		entry->setPosition(System::random(MAXCOORD * 2) + MINCOORD, 0, System::random(MAXCOORD * 2) + MINCOORD);

		quadTree->update(entry);
		quadTree->inRange(entry, 512);
#else
		Locker locker(entry);

		Locker locker2(&qtMutex);

		entry->setPosition(System::random(MAXCOORD * 2) + MINCOORD, 0, System::random(MAXCOORD * 2) + MINCOORD);

		qt->update(entry);
		qt->inRange(entry, 512);
#endif
	}
};

void testQTSTM() {
#ifdef WITH_STM
	Reference<Transaction*> transaction = TransactionalMemoryManager::instance()->startTransaction();
#endif

	TransactionalReference<QuadTree*> qt = new QuadTree(MINCOORD, MINCOORD, MAXCOORD, MAXCOORD);
	Vector<Reference<QuadTreeEntry*> > objects;

	printf("Creating %d objects..\n", OBJECTCOUNT);

	int j = 0;

	for (int i = 0; i < OBJECTCOUNT; ++i) {
		Reference<QuadTreeEntry*> entry = new QuadTreeEntry();
//		entry->depl

		//entry->initializePosition(System::random(MAXCOORD * 2) + MINCOORD, 0, System::random(MAXCOORD * 2) + MINCOORD);
		entry->_setObjectID(i);
		entry->initializePosition(0, 0, 0);
		/*float x = i % MAXCOORD;

		entry->initializePosition((MINCOORD + i) % MAXCOORD, 0, MINCOORD + (++j % MAXCOORD));*/

		objects.add(entry);
	}

	printf("Inserting objects into QuadTree...\n");

	for (int i = 0; i < objects.size(); ++i) {
		Reference<QuadTreeEntry*>& obj = objects.get(i);

		qt->insert(obj);

		qt->inRange(obj, 512);
	}

	int maxInRangeObjects = 0;

	for (int i = 0; i < objects.size(); ++i) {
		Reference<QuadTreeEntry*>& obj = objects.get(i);

		if (obj->inRangeObjectCount() > maxInRangeObjects)
			maxInRangeObjects = obj->inRangeObjectCount();
	}

	printf("maxInRangeObjects = %d\n", maxInRangeObjects);

#ifdef WITH_STM
	printf("Commiting pure transaction...\n");

	TransactionalMemoryManager::commitPureTransaction(transaction);
#endif


	printf("starting tasks\n");

	Time start;

	int totalTasks = 0;

	//Thread::sleep(3000);

	for (int i = 0; i < ITERATIONS; ++i) {
		int scheduledTasks = Core::getTaskManager()->getScheduledTaskSize();
		int executedTasks = Core::getTaskManager()->getExecutingTaskSize();

		if (scheduledTasks != 0 && executedTasks != 0) {
			Thread::sleep(1000);
		}

		int taskToSchedule = 5000;
		int taskToExecute = 5000;
#ifdef WITH_STM
		if (scheduledTasks > 500)
			taskToSchedule = 0;
		else if (scheduledTasks < 100)
			taskToSchedule = 100;

		if (executedTasks > 500)
			taskToExecute = 0;
		else if (executedTasks < 100)
			taskToExecute = 100;
#else
		if (scheduledTasks > 5000)
			taskToSchedule = 0;
		else if (scheduledTasks < 1000)
			taskToSchedule = 50000;

		if (executedTasks > 5000)
			taskToExecute = 0;
		else if (executedTasks < 1000)
			taskToExecute = 50000;
#endif

#ifdef WITH_STM
		transaction = TransactionalMemoryManager::instance()->startTransaction();
#endif

		for (int i = 0; i < taskToSchedule; ++i) {
			Reference<Task*> task = new QTMoveTask(objects.get(System::random(objects.size() - 1)), qt);

			Core::getTaskManager()->scheduleTask(task, System::random(2000));

			++totalTasks;
		}

		for (int i = 0; i < taskToExecute; ++i) {
			Reference<Task*> task = new QTMoveTask(objects.get(System::random(objects.size() - 1)), qt);

			++totalTasks;

			Core::getTaskManager()->executeTask(task);
		}

#ifdef WITH_STM
		TransactionalMemoryManager::commitPureTransaction(transaction);
#endif
	}

	/*for (int i = 0; i < references.size(); ++i) {
			TestStmTask* object = references.get(i).get();

			printf("%i\n", object->get());
		}*/

	printf("queued %d tasks\n", totalTasks);

	//TransactionalMemoryManager::commitPureTransaction();

	int scheduledTasks = Core::getTaskManager()->getScheduledTaskSize();
	int executedTasks = Core::getTaskManager()->getExecutingTaskSize();

	printf("waiting to finsih...");

	while (scheduledTasks != 0 || executedTasks != 0) {
		scheduledTasks = Core::getTaskManager()->getScheduledTaskSize();
		executedTasks = Core::getTaskManager()->getExecutingTaskSize();
		Thread::sleep(1000);

		printf(".");
	}

	printf("finished in %lld\n", start.miliDifference());

	printf("\n");

#ifdef WITH_STM
	TransactionalMemoryManager::instance()->printStatistics();
#endif

	exit(0);
}


#endif /* QTSTMTEST_H_ */
