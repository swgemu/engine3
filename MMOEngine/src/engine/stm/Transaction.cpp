/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/core/Core.h"

#include "TransactionalMemoryManager.h"

#include "task/TransactionalTaskManager.h"

#include "TransactionalObjectHandle.h"
#include "TransactionalObjectHeader.h"

#include "Transaction.h"

using namespace engine::stm;

Transaction::Transaction(int id) : Logger() {
	status = INITIAL;

	commitTime = 0;
	runTime = 0;
	commitAttempts = 1;

	task = NULL;

	Command* command = (TransactionalTaskManager*) Core::getTaskManager();
	commands.add(command);

	command = TransactionalMemoryManager::instance()->getObjectManager();
	commands.add(command);

	command = TransactionalMemoryManager::instance()->getSocketManager();
	commands.add(command);

	tid = id;

	setLogging(false);
	setGlobalLogging(true);
}

Transaction::~Transaction() {
	reset();

	debug("deleted");
}

bool Transaction::start() {
	return start(task);
}

bool Transaction::start(Task* task) {
	if (!setState(INITIAL, UNDECIDED)) {
		debug("failed to start transaction (state " + String::valueOf(status) + ")");

		return false;
	}

	assert(task != NULL && "task was NULL");

	Transaction::task = task;

	helperTransaction = NULL;

	String threadName = Thread::getCurrentThread()->getName();
	setLoggingName("Transaction " + String::valueOf(tid) + "(" + threadName + ")");

	TransactionalMemoryManager::instance()->startTransaction(this);

	uint64 startTime = System::getMikroTime();

	try {
		debug("starting transaction");

		task->run();
	} catch (TransactionAbortedException& e) {
		abort();
	} catch (Exception& e) {
		e.printStackTrace();

		abort();
	}

	runTime += System::getMikroTime() - startTime;

	return true;
}

bool Transaction::commit() {
	debug("commiting..");

	uint64 startTime = System::getMikroTime();

	int readOnlyObjectsCount = readOnlyObjects.size();
	int readWriteObjectsCount = readWriteObjects.size();

	bool commited = false;

	//if (tid == 1 || commitAttempts > 0)
		commited = doCommit();

	commitTime += System::getMikroTime() - startTime;

	if (commited) {
		debug("commited (" + String::valueOf(runTime) + "Us / " + String::valueOf(commitTime) + "Us, "
				+ String::valueOf(commitAttempts) + " tries, R/W objects "
				+ readOnlyObjectsCount + " / " + readWriteObjectsCount +")");

		doHelpTransactions();
	} else {
		doAbort();

		if (helperTransaction.compareAndSet(NULL, this)) {
			Thread::yield();

			debug("helping self");

			commited = doRetry();
		}
	}

	Thread::yield();

	return commited;
}

bool Transaction::doCommit() {
	if (!isUndecided()) {
		debug("transaction not in UNDECIDED state on commit");
		return false;
	}

	if (!acquireReadWriteObjects()) {
		debug("failed to acquire read write objects");
		return false;
	}

	if (!setState(UNDECIDED, READ_CHECKING)) {
		debug("status change UNDECIED -> READ CHECKING failed");
		return false;
	}

	if (!validateReadOnlyObjects()) {
		debug("failed to validate read only objects");
		return false;
	}

	if (!setState(READ_CHECKING, COMMITTED)) {
		debug("status change READ CHECKING -> COMMITED failed");
		return false;
	}

	for (int i = 0; i < commands.size(); ++i) {
		Command* command = commands.get(i);

		command->execute();
	}

	releaseReadWriteObjects();

	TransactionalMemoryManager::instance()->commitTransaction();

	return true;
}

void Transaction::abort() {
	int currentStatus = status;

	if (currentStatus == COMMITTED || currentStatus == INITIAL)
		return;

	setState(currentStatus, ABORTED);

	if (status == ABORTED)
		debug("aborting");
	else
		debug("aborting failed");
}

void Transaction::doAbort() {
	status = ABORTED;

	discardReadWriteObjects();

	for (int i = 0; i < commands.size(); ++i) {
		Command* command = commands.get(i);

		command->undo();
	}

	debug("aborted (" + String::valueOf(runTime) + "Us / " + String::valueOf(commitTime) + "Us, "
			+ String::valueOf(commitAttempts) + " tries, R/W objects "
			+ readOnlyObjects.size() + " / " + readWriteObjects.size() +")");

	if (commitAttempts > 1)
		TransactionalMemoryManager::instance()->retryTransaction();

	++commitAttempts;

	reset();

	TransactionalMemoryManager::instance()->abortTransaction();
}

bool Transaction::doRetry() {
	TransactionalMemoryManager::instance()->getTaskManager()->executeTask(task);

	return false;

	/*if (!start())
		return false;

	return commit();*/
}

void Transaction::doHelpTransactions() {
	for (int i = 0; i < helpedTransactions.size(); ++i) {
		//Transaction* helpedTransaction = helpedTransactions.get(i);
		Reference<Transaction*> helpedTransaction = helpedTransactions.get(i);

		while (!helpedTransaction->isInitial() && !helpedTransaction->isCommited()) {
			Thread::yield();
		}

		debug("helping transaction " + helpedTransaction->getLoggingName());

		helpedTransaction->doRetry();
	}
}

void Transaction::reset() {
	debug("reset");

	openedObjets.removeAll();

	localObjectCache.removeAll();

	for (int i = 0; i < readOnlyObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readOnlyObjects.get(i);
		delete handle;
	}

	readOnlyObjects.removeAll();

	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readWriteObjects.get(i);
		delete handle;
	}

	readWriteObjects.removeAll();

	status = INITIAL;
}

bool Transaction::acquireReadWriteObjects() {
	debug("acquiring read/write objects");

	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readWriteObjects.get(i);

		if (handle->hasObjectChanged()) {
			debug("object '" + handle->getObjectLocalCopy()->toString() + "' has changed on acquiring RW objects");
			return false;
		}

		while (!handle->acquireHeader(this)) {
			Transaction* competingTransaction = handle->getCompetingTransaction();
			if (competingTransaction == NULL) {
				debug("conflict with already released transaction");
				return false;
			}

			if (!resolveConflict(competingTransaction))
				return false;

			Thread::yield();
		}
	}

	/*for (int i = 0; i < readOnlyObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readOnlyObjects.get(i);

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
	}*/

	debug("finished acquiring read/write objects");

	return true;
}

void Transaction::releaseReadWriteObjects() {
	debug("releasing read/write objects");

	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readWriteObjects.get(i);

		handle->releaseHeader();
	}

	debug("finished releasing read/write objects");
}

bool Transaction::validateReadOnlyObjects() {
	debug("validating read only objects");

	for (int i = 0; i < readOnlyObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readOnlyObjects.get(i);

		if (handle->hasObjectChanged()) {
			debug("object has changed on validating RO objects");
			return false;
		}

		Transaction* competitorTransaction = handle->getCompetingTransaction();

		if (competitorTransaction != NULL && competitorTransaction->isReadChecking()) {
			if (!resolveConflict(competitorTransaction))
				return false;
		}
	}

	debug("finished validating read only objects");

	return true;
}

void Transaction::discardReadWriteObjects() {
	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readWriteObjects.get(i);

		handle->discardHeader(this);
	}
}

bool Transaction::resolveConflict(Transaction* transaction) {
	if (transaction->compareTo(this) < 0) {
		debug("aborting conflicting transction " + transaction->getLoggingName());

		helpTransaction(transaction);

		return transaction->isAborted();
	} else {
		debug("aborting self due to conflict");

		//transaction->helpTransaction(this);

		return false;
	}
}

void Transaction::helpTransaction(Transaction* transaction) {
	transaction->abort();

	if (transaction->isAborted()) {
		if (!transaction->setHelperTransaction((this)))
			return;

		debug("adding helped transaction " + transaction->getLoggingName());

		helpedTransactions.add(transaction);
	}
}

int Transaction::compareTo(Transaction* transaction) {
	if (transaction > this)
		return 1;
	else if (transaction < this)
		return -1;
	else
		return 0;
}

Transaction* Transaction::currentTransaction() {
	return TransactionalMemoryManager::instance()->getTransaction();
}

String Transaction::toString() {
	return "Transaction [" + Thread::getCurrentThread()->getName() + "] commited in " + Long::toString(commitTime) + " usec with "
			+ String::valueOf(commitAttempts) + " attempts";
}

bool Transaction::setState(int newstate) {
	return setState(status.get(), newstate);
}

bool Transaction::setState(int currentstate, int newstate) {
	return status.compareAndSet(currentstate, newstate);
}
