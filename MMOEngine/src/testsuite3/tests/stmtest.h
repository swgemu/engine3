/*
Copyright (C) 2007 <SWGEmu>

This File is part of Core3.

This program is free software; you can redistribute
it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software
Foundation; either version 2 of the License,
or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General
Public License along with this program; if not, write to
the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Linking Engine3 statically or dynamically with other modules
is making a combined work based on Engine3.
Thus, the terms and conditions of the GNU Lesser General Public License
cover the whole combination.

In addition, as a special exception, the copyright holders of Engine3
give you permission to combine Engine3 program with free software
programs or libraries that are released under the GNU LGPL and with
code included in the standard release of Core3 under the GNU LGPL
license (or modified versions of such code, with unchanged license).
You may copy and distribute such a system following the terms of the
GNU LGPL for Engine3 and the licenses of the other code concerned,
provided that you include the source code of that other code when
and as the GNU LGPL requires distribution of source code.

Note that people who make modified versions of Engine3 are not obligated
to grant this special exception for their modified versions;
it is their choice whether to do so. The GNU Lesser General Public License
gives permission to release a modified version without this exception;
this exception also makes it possible to release a modified version
which carries forward this exception.
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
			assert(references->get(i) != NULL);
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
