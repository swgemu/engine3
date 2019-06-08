/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/engine.h"

#include "TestClass.h"

#ifdef WITH_STM

class TestStmTask : public Task {
	Vector<TransactionalReference<TestClass*> >* references;

public:
	TestStmTask(Vector<TransactionalReference<TestClass*> >* refs) {
		references = refs;

		/*for (int i = 0; i < references->size(); ++i) {
			assert(references->get(i) != nullptr);
		}*/
	}

	void run() {
		Reference<Task*> task = new TestStmTask(references);
		if (System::random(1) == 0)
			task->schedule(System::random(10000));
		else
			task->execute();

		for (int i = 0; i < 10; ++i) {
			int index = System::random(references->size() - 1);
			TransactionalReference<TestClass*>& reference = references->get(index);

			TestClass* object = reference.getForUpdate();
			object->increment();
		}
	}
};

#endif

void testTransactions() {
#ifdef WITH_STM
	Reference<Transaction*> transaction = TransactionalMemoryManager::instance()->startTransaction();

	Vector<TransactionalReference<TestClass*> > references;

	printf("creating objects\n");

	for (int i = 0; i < 10000; ++i)
		references.add(new TestClass(1));

	printf("adding tasks\n");

	int totalTasks = 0;

	for (int i = 0; i < 300000; ++i) {
		Reference<Task*> task = new TestStmTask(&references);
		task->execute();
		++totalTasks;
		//Core::getTaskManager()->scheduleTask(task, 1000);
	}

	TransactionalMemoryManager::commitPureTransaction(transaction);

	printf("starting tasks\n");

	while (Core::getTaskManager()->getExecutingTaskSize() != 0)
		Thread::sleep(1000);

/*	for (int i = 0; i < 20; ++i) {
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
			Task* task = new TestStmTask(&references);

			Core::getTaskManager()->scheduleTask(task, System::random(2000));

			++totalTasks;
		}

		for (int i = 0; i < taskToExecute; ++i) {
			Reference<Task*> task = new TestStmTask(&references);

			++totalTasks;

			Core::getTaskManager()->executeTask(task);
		}

		TransactionalMemoryManager::commitPureTransaction(transaction);
	}
*/
	/*for (int i = 0; i < references.size(); ++i) {
		TestStmTask* object = references.get(i).get();

		printf("%i\n", object->get());
	}*/

	printf("queued %d tasks\n", totalTasks);

	//TransactionalMemoryManager::commitPureTransaction();

	Thread::sleep(10000);
#endif

	exit(0);
}
