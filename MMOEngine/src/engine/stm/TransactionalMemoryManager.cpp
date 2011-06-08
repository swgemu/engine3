/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/core/Core.h"

#include "TransactionalMemoryManager.h"

using namespace engine::stm;

class PureTask : public Task {
public:
	void run() {
		//Logger::console.warning("pure transaction called");
	}
};

class TransactionalMemoryManagerStatisticsTask : public Thread {
public:
	void run() {
		Thread::sleep(1000);

		while (true) {
			TransactionalMemoryManager::instance()->printStatistics();

			Thread::sleep(100000);
		}
	}
};

//Vector<bool> commitedTrans;
AtomicBoolean initializationTransactionStarted;

TransactionalMemoryManager::TransactionalMemoryManager() : Logger("TransactionalMemoryManager") {
	setInstance(this);

	taskManager = (TransactionalTaskManager*) Core::getTaskManager();

	objectManager = (TransactionalObjectManager*) Core::getObjectBroker();

	socketManager = new TransactionalSocketManager();

	baseClientManager = new TransactionalBaseClientManager();

	Thread* task = new TransactionalMemoryManagerStatisticsTask();
	task->start();

	/*for (int i = 0; i < 101000; ++i)
		commitedTrans.add(false);*/

	initializationTransactionStarted = false;

	setInfoLogLevel();
	setGlobalLogging(true);
}

TransactionalMemoryManager::~TransactionalMemoryManager() {
	/*printf("Undone transactions: ");

	for (unsigned int i = 1; i < transactionID.get(); ++i) {
		if (!commitedTrans.get(i))
			printf("%i ", i);
	}

	printf("\n");*/

	setInstance(NULL);
}

void TransactionalMemoryManager::commitPureTransaction() {
	Reference<Task*> task = new PureTask();

	task->execute();
}

Transaction* TransactionalMemoryManager::getTransaction() {
	Transaction* transaction = currentTransaction.get();

	if (transaction == NULL) {
		transaction = new Transaction(transactionID.increment());

		//transaction->info("created");

		currentTransaction.set(transaction);
	}

	return transaction;
}

void TransactionalMemoryManager::startTransaction(Transaction* transaction) {
	Transaction* current = currentTransaction.get();

/*	while (current != NULL) {
		current = currentTransaction.get();

		Thread::sleep(10);
	}*/

	/*while (commitedTrans.size() < transaction->getIdentifier())
		commitedTrans.add(false);*/

	//commitedTrans.add(transaction->getIdentifier(), false);

	if (transaction->getIdentifier() != 1) {
		while (!initializationTransactionStarted.get()) {
			Thread::sleep(1000);

			current = currentTransaction.get();
		}
	}


	assert(current == NULL || current == transaction);

	currentTransaction.set(transaction);

	startedTransactions.increment();
}

void TransactionalMemoryManager::commitTransaction() {
	Transaction* transaction = currentTransaction.get();
	//commitedTrans.set(transaction->getIdentifier(), true);

	debug("Executing tasks: " + String::valueOf(Core::getTaskManager()->getExecutingTaskSize()));

	reclaimObjects(1000, 1000);

	commitedTransactions.increment();

	if (transaction->getIdentifier() == 1)
		initializationTransactionStarted.compareAndSet(false, true);

	currentTransaction.set(NULL);
}

void TransactionalMemoryManager::abortTransaction() {
	currentTransaction.set(NULL);

	abortedTransactions.increment();
}

void TransactionalMemoryManager::retryTransaction() {
	retryConflicts.increment();
}

void TransactionalMemoryManager::reclaim(Object* object) {
	if (object->_isGettingDestroyed())
		return;

	Vector<Object*>* objects = getReclamationList();

	objects->add(object);

	object->_setDestroying(true);
}

void TransactionalMemoryManager::reclaimObjects(int objectsToSpare, int maxObjectsToReclaim) {
	Vector<Object*>* objects = getReclamationList();

	int objectsReclaimed = 0;

	uint64 startTime = System::getMikroTime();

	while (objects->size() > objectsToSpare) {
		Object* obj = objects->remove(0);

		if (obj->getReferenceCount() == 0) {
			MemoryManager::reclaim(obj);

			if (++objectsReclaimed == maxObjectsToReclaim)
				break;
		} else
			obj->_setDestroying(false);
	}

	debug(String::valueOf(objectsReclaimed) + " objects were reclaimed in "
			+ String::valueOf(System::getMikroTime() - startTime) + " Us");
}

 Vector<Object*>* TransactionalMemoryManager::getReclamationList() {
	Vector<Object*>* objects = reclamationList.get();
	if (objects == NULL) {
		objects = new Vector<Object*>();

		reclamationList.set(objects);
	}

	return objects;
 }

void TransactionalMemoryManager::printStatistics() {
	if (startedTransactions.get() == 0)
		return;

	TaskManager* taskManager = Core::getTaskManager();

	StringBuffer str;
	str << "transactions(started " << startedTransactions.get() << ", "
			<< "commited " << commitedTransactions.get() << ", "
			<< "retried " << retryConflicts.get() << ", "
			<< "aborted " << abortedTransactions.get() << ") - tasks ("
			<< "exectuing " << taskManager->getExecutingTaskSize() << ", "
			<< "scheduled " << taskManager->getScheduledTaskSize() << ")";


	info(str);

	startedTransactions.set(0);
	commitedTransactions.set(0);
	abortedTransactions.set(0);
	retryConflicts.set(0);
}
