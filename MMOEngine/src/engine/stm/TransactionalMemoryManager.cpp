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
		Logger::console.warning("pure transaction called");
	}
};

class TransactionalMemoryManagerStatisticsTask : public Task {
public:
	void run() {
		TransactionalMemoryManager::instance()->printStatistics();
	}
};

//Vector<bool> commitedTrans;
bool initializationTransactionStarted;

TransactionalMemoryManager::TransactionalMemoryManager() : Logger("TransactionalMemoryManager") {
	objectManager = (TransactionalObjectManager*) Core::getObjectBroker();

	socketManager = new TransactionalSocketManager();

	Task* task = new TransactionalMemoryManagerStatisticsTask();
	task->schedulePeriodic(1000, 1000);

	/*for (int i = 0; i < 101000; ++i)
		commitedTrans.add(false);*/

	initializationTransactionStarted = false;

	setLogging(false);
	setGlobalLogging(true);
}

TransactionalMemoryManager::~TransactionalMemoryManager() {
	/*printf("Undone transactions: ");

	for (unsigned int i = 1; i < transactionID.get(); ++i) {
		if (!commitedTrans.get(i))
			printf("%i ", i);
	}

	printf("\n");*/
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
		while (!initializationTransactionStarted) {
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
		initializationTransactionStarted = true;
}

void TransactionalMemoryManager::abortTransaction() {
	currentTransaction.set(NULL);

	abortedTransactions.increment();
}

void TransactionalMemoryManager::printStatistics() {
	if (startedTransactions.get() <= 1)
		return;

	TaskManager* taskManager = Core::getTaskManager();

	StringBuffer str;
	str << "transactions(started " << String::valueOf(startedTransactions.get() - 1) << ", "
			<< "commited " << String::valueOf(commitedTransactions.get() - 1) << ", "
			<< "aborted " << abortedTransactions.get() << ") - tasks ("
			<< "exectuing " << taskManager->getExecutingTaskSize() << ", "
			<< "scheduled " << taskManager->getScheduledTaskSize() << ")";


	info(str, true);

	startedTransactions.set(0);
	commitedTransactions.set(0);
	abortedTransactions.set(0);
}
