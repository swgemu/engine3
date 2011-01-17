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

class TransactionalMemoryManagerStatisticsTask : public ReentrantTask {
public:
	void run() {
		TransactionalMemoryManager::instance()->printStatistics();

		Core::getTaskManager()->scheduleTask(this, 1000);
	}
};

TransactionalMemoryManager::TransactionalMemoryManager() : Logger("TransactionalMemoryManager") {
	objectManager = new TransactionalObjectManager();

	socketManager = new TransactionalSocketManager();

	Core::getTaskManager()->scheduleTask(new TransactionalMemoryManagerStatisticsTask(), 1000);

	setLogging(true);
	setGlobalLogging(true);
}

void TransactionalMemoryManager::commitPureTransaction() {
	Reference<Task*> task = new PureTask();

	task->execute();
}

Transaction* TransactionalMemoryManager::getTransaction() {
	Transaction* transaction = currentTransaction.get();

	if (transaction == NULL) {
		transaction = new Transaction();

		//transaction->info("created");

		currentTransaction.set(transaction);
	}

	return transaction;
}

void TransactionalMemoryManager::startTransaction(Transaction* transaction) {
	Transaction* current = currentTransaction.get();

	assert(current == NULL || current == transaction);

	currentTransaction.set(transaction);

	startedTransactions.increment();
}

void TransactionalMemoryManager::commitTransaction() {
	currentTransaction.set(NULL);

	commitedTransactions.increment();
}

void TransactionalMemoryManager::abortTransaction() {
	currentTransaction.set(NULL);

	abortedTransactions.increment();
}

void TransactionalMemoryManager::printStatistics() {
	if (startedTransactions.get() <= 1)
		return;

	StringBuffer str;
	str << "started " << startedTransactions.get() << ", " << "commited " << commitedTransactions.get() << ", "
			<< "aborted " << abortedTransactions.get();

	info(str);

	startedTransactions.set(0);
	commitedTransactions.set(0);
	abortedTransactions.set(0);
}
