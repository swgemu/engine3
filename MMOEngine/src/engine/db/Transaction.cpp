/*
 * Transaction.cpp
 *
 *  Created on: 14/02/2010
 *      Author: victor
 */

#include "Transaction.h"

TransactionConfig TransactionConfig::DEFAULT;

int Transaction::abort() {
	return transaction->abort(transaction);
}

/**
 * End the transaction.
 */

int Transaction::commit(uint32 flags) {
	return transaction->commit(transaction, flags);
}
