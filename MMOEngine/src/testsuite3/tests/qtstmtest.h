/*
 * qtstmtest.h
 *
 *  Created on: 11/11/2011
 *      Author: victor
 */

#ifndef QTSTMTEST_H_
#define QTSTMTEST_H_

#include "engine/engine.h"

#define OBJECTCOUNT 10000
#define MINCOORD -8192
#define MAXCOORD 8192


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
		//Task* task = new TestTask(references);
		//QuadTreeEntry* move = entry.getForUpdate();
		QuadTree* quadTree = qt.getForUpdate();

		entry->setPosition(System::random(MAXCOORD * 2) + MINCOORD, 0, System::random(MAXCOORD * 2) + MINCOORD);

		quadTree->update(entry);
		quadTree->inRange(entry, 512);
#endif
	}
};

void testQTSTM() {
#ifdef WITH_STM
	Reference<Transaction*> transaction = TransactionalMemoryManager::instance()->startTransaction();

	TransactionalReference<QuadTree*> qt = new QuadTree(MINCOORD, MINCOORD, MAXCOORD, MAXCOORD);
	Vector<Reference<QuadTreeEntry*> > objects;

	printf("Creating %d objects..\n", OBJECTCOUNT);

	for (int i = 0; i < OBJECTCOUNT; ++i) {
		Reference<QuadTreeEntry*> entry = new QuadTreeEntry();
//		entry->depl

		entry->initializePosition(System::random(MAXCOORD * 2) + MINCOORD, 0, System::random(MAXCOORD * 2) + MINCOORD);

		objects.add(entry);
	}

	printf("Inserting objects into QuadTree...\n");

	for (int i = 0; i < objects.size(); ++i) {
		Reference<QuadTreeEntry*>& obj = objects.get(i);

		qt->insert(obj);

		qt->inRange(obj, 512);
	}

	printf("Commiting pure transaction...\n");

	TransactionalMemoryManager::commitPureTransaction(transaction);


	printf("starting tasks\n");

	int totalTasks = 0;

	//Thread::sleep(3000);

	for (int i = 0; i < 60; ++i) {
		Thread::sleep(1000);

		int scheduledTasks = Core::getTaskManager()->getScheduledTaskSize();
		int executedTasks = Core::getTaskManager()->getExecutingTaskSize();

		int taskToSchedule = 500;
		int taskToExecute = 500;

		if (scheduledTasks > 5000)
			taskToSchedule = 0;
		else if (scheduledTasks < 1000)
			taskToSchedule = 3000;

		if (executedTasks > 5000)
			taskToExecute = 0;
		else if (executedTasks < 1000)
			taskToExecute = 3000;

		transaction = TransactionalMemoryManager::instance()->startTransaction();

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

		TransactionalMemoryManager::commitPureTransaction(transaction);
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

	printf("\n");

	TransactionalMemoryManager::instance()->printStatistics();

	exit(0);

#endif
}


#endif /* QTSTMTEST_H_ */
