/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "TransactionalMemoryManager.h"

#include "TransactionalObjectHandle.h"
#include "TransactionalObjectHeader.h"

#include "Transaction.h"

using namespace engine::stm;

Transaction::~Transaction() {
	for (int i = 0; i < readOnlyObjects.size(); ++i) {
		TransactionalObjectHandle<TransactionalObject>* handle = readOnlyObjects.get(i);
		delete handle;
	}

	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandle<TransactionalObject>* handle = readWriteObjects.get(i);
		delete handle;
	}

	TransactionalMemoryManager::instance()->clearTransaction();
}

bool Transaction::commit() {
	if (!isUndecided()) {
		abort();
		return false;
	}

	if (!acquireReadWriteObjects())
		return false;

	if (!setState(UNDECIDED, READ_CHECKING)) {
		abort();
		return false;
	}

	if (!validateReadOnlyObjects())
		return false;

	if (!setState(READ_CHECKING, COMMITTED)) {
		abort();
		return false;
	}

	releaseReadWriteObjects();

	return true;
}

bool Transaction::acquireReadWriteObjects() {
	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandle<TransactionalObject>* handle = readWriteObjects.get(i);

		if (!handle->acquireHeader(this)) {
			Transaction* competingTransaction = handle->getCompetingTransaction();

			if (!resolveConflict(competingTransaction))
				return false;
		}
	}

	for (int i = 0; i < readOnlyObjects.size(); ++i) {
		TransactionalObjectHandle<TransactionalObject>* handle = readOnlyObjects.get(i);

		if (handle->hasObjectContentChanged()) {
			readOnlyObjects.remove(i--);
			readWriteObjects.add(handle);

			if (!handle->acquireHeader(this)) {
				Transaction* competingTransaction = handle->getCompetingTransaction();

				if (!resolveConflict(competingTransaction)) {
					return false;
				}
			}
		}
	}

	return true;
}

void Transaction::releaseReadWriteObjects() {
	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandle<TransactionalObject>* handle = readWriteObjects.get(i);
		handle->releaseHeader();

		delete handle;
	}

	readWriteObjects.removeAll();
}

bool Transaction::validateReadOnlyObjects() {
	for (int i = 0; i < readOnlyObjects.size(); ++i) {
		TransactionalObjectHandle<TransactionalObject>* handle = readOnlyObjects.get(i);

		if (handle->hasObjectChanged()) {
			abort();
			return false;
		}

		Transaction* competitorTransaction = handle->getCompetingTransaction();

		if (competitorTransaction != NULL && competitorTransaction->isReadChecking()) {
			if (!resolveConflict(competitorTransaction))
				return false;
		}
	}

	return true;
}

void Transaction::abort() {
	status = ABORTED;
}

bool Transaction::resolveConflict(Transaction* transaction) {
	if (transaction > this) {
		transaction->abort();

		return true;
	} else {
		this->abort();

		return false;
	}
}

Transaction* Transaction::currentTransaction() {
	return TransactionalMemoryManager::instance()->getTransaction();
}

bool Transaction::setState(int currentstate, int newstate) {
	return status.compareAndSet(currentstate, newstate);
}
