/*
 * Transaction.cpp
 *
 *  Created on: 14/02/2010
 *      Author: victor
 */

#include "BTransaction.h"

using namespace engine::db::berkley;

TransactionConfig TransactionConfig::DEFAULT;

int Transaction::abort() {
	int ret = transaction->abort(transaction);

	transaction = nullptr;

	delete this;

	return ret;
}

/**
 * End the transaction.
 */

int Transaction::commit(uint32 flags) {
	int ret = transaction->commit(transaction, flags);

	transaction = nullptr;

	delete this;

	return ret;
}
