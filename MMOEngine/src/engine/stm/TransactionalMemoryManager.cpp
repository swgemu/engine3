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
	assert(currentTransaction.get() == NULL);

	currentTransaction.set(transaction);
}

void TransactionalMemoryManager::clearTransaction() {
	currentTransaction.set(NULL);
}
