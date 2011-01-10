/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "TransactionalMemoryManager.h"

using namespace engine::stm;

TransactionalMemoryManager::TransactionalMemoryManager() {
	objectManager = new TransactionalObjectManager();

	socketManager = new TransactionalSocketManager();
}

class PureTask : public Task {
public:
	void run() {
		Logger::console.warning("pure transaction called");
	}
};

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

void TransactionalMemoryManager::setTransaction(Transaction* transaction) {
	Transaction* current = currentTransaction.get();

	assert(current == NULL || current == transaction);

	currentTransaction.set(transaction);
}

void TransactionalMemoryManager::clearTransaction() {
	currentTransaction.set(NULL);
}
