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

			Thread::sleep(1000);
		}
	}
};

//Vector<bool> commitedTrans;
AtomicBoolean initializationTransactionStarted;

TransactionalMemoryManager::TransactionalMemoryManager() : Logger("TransactionalMemoryManager") {
	setInstance(this);

	objectManager = (TransactionalObjectManager*) Core::getObjectBroker();

	socketManager = new TransactionalSocketManager();

	Thread* task = new TransactionalMemoryManagerStatisticsTask();
	task->start();

	/*for (int i = 0; i < 101000; ++i)
		commitedTrans.add(false);*/

	initializationTransactionStarted = false;

	setLogging(false);
	setGlobalLogging(false);
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

	/*while (commitedTrans.size() < transaction->getIdentifier())
		commitedTrans.add(false);*/

	//commitedTrans.add(transaction->getIdentifier(), false);

	assert(current == NULL || current == transaction);

	if (transaction->getIdentifier() != 1) {
		while (!initializationTransactionStarted.get()) {
			Thread::sleep(1000);
		}
	}

	currentTransaction.set(transaction);

	startedTransactions.increment();
}

void TransactionalMemoryManager::commitTransaction() {
	Transaction* transaction = currentTransaction.get();
	//commitedTrans.set(transaction->getIdentifier(), true);

	info("Executing tasks: " + String::valueOf(Core::getTaskManager()->getExecutingTaskSize()));

	currentTransaction.set(NULL);

	commitedTransactions.increment();

	if (transaction->getIdentifier() == 1)
		initializationTransactionStarted.compareAndSet(false, true);
}

void TransactionalMemoryManager::abortTransaction() {
	currentTransaction.set(NULL);

	abortedTransactions.increment();
}

void TransactionalMemoryManager::reclaim(Object* object) {
	Vector<Object*>* objects = getReclamationList();

	objects->add(object);

	if (objects->size() > 5000 && !isReclaiming.get()) {
		isReclaiming.set(true);

		while (objects->size() > 5000) {
			Object* obj = objects->remove(0);

			MemoryManager::reclaim(obj);
		}

		isReclaiming.set(false);
	}
}

void TransactionalMemoryManager::reclaimAll() {
	Vector<Object*>* objects = getReclamationList();

	isReclaiming.set(true);

	while (!objects->isEmpty()) {
		Object* obj = objects->remove(0);

		MemoryManager::reclaim(obj);
	}

	isReclaiming.set(false);
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
			<< "aborted " << abortedTransactions.get() << ") - tasks ("
			<< "exectuing " << taskManager->getExecutingTaskSize() << ", "
			<< "scheduled " << taskManager->getScheduledTaskSize() << ")";


	info(str, true);

	startedTransactions.set(0);
	commitedTransactions.set(0);
	abortedTransactions.set(0);
}
