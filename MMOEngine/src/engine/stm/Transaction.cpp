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

	info("deleted");
}

bool Transaction::start() {
	return start(task);
}

bool Transaction::start(Task* task) {
	if (!setState(INITIAL, UNDECIDED)) {
		info("failed to start transaction (state " + String::valueOf(status) + ")");

		return false;
	}

	assert(task != NULL && "task was NULL");

	Transaction::task = task;

	String threadName = Thread::getCurrentThread()->getName();

	setLoggingName("Transaction " + String::valueOf(tid) + "(" + threadName + ")");

	info("starting transaction");

	TransactionalMemoryManager::instance()->startTransaction(this);

	helperTransaction = NULL;

	uint64 startTime = System::getMikroTime();

	try {
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
	info("commiting..");

	uint64 startTime = System::getMikroTime();

	int readOnlyObjectsCount = readOnlyObjects.size();
	int readWriteObjectsCount = readWriteObjects.size();

	bool commited = doCommit();

	commitTime += System::getMikroTime() - startTime;

	if (commited) {
		info("commited (" + String::valueOf(runTime / 1000) + "ms / " + String::valueOf(commitTime) + "Us, "
				+ String::valueOf(commitAttempts) + " tries, R/W objects "
				+readOnlyObjectsCount + " / " + readWriteObjectsCount +")");

		doHelpTransactions();

		//delete this;
	} else {
		doAbort();

		if (helperTransaction.compareAndSet(NULL, this)) {
			Thread::yield();

			info("helping self");

			if (!start())
				return false;

			return commit();
		}
	}

	Thread::yield();

	return commited;
}

bool Transaction::doCommit() {
	if (!isUndecided()) {
		info("transaction not in UNDECIDED state on commit");
		return false;
	}

	if (!acquireReadWriteObjects()) {
		info("failed to acquire read write objects");
		return false;
	}

	if (!setState(UNDECIDED, READ_CHECKING)) {
		info("status change UNDECIED -> READ CHECKING failed");
		return false;
	}

	if (!validateReadOnlyObjects()) {
		info("failed to validate read only objects");
		return false;
	}

	if (!setState(READ_CHECKING, COMMITTED)) {
		info("status change READ CHECKING -> COMMITED failed");
		return false;
	}

	releaseReadWriteObjects();

	for (int i = 0; i < commands.size(); ++i) {
		Command* command = commands.get(i);

		command->execute();
	}

	TransactionalMemoryManager::instance()->commitTransaction();

	return true;
}

void Transaction::abort() {
	int currentStatus = status;

	if (currentStatus == COMMITTED || currentStatus == INITIAL)
		return;

	setState(currentStatus, ABORTED);

	if (status == ABORTED)
		info("aborting");
	else
		info("aborting failed");
}

void Transaction::doAbort() {
	status = ABORTED;

	discardReadWriteObjects();

	for (int i = 0; i < commands.size(); ++i) {
		Command* command = commands.get(i);

		command->undo();
	}

	info("aborted");

	++commitAttempts;

	reset();

	TransactionalMemoryManager::instance()->abortTransaction();
}

void Transaction::doHelpTransactions() {
	for (int i = 0; i < helpedTransactions.size(); ++i) {
		//Transaction* helpedTransaction = helpedTransactions.get(i);
		Reference<Transaction*> helpedTransaction = helpedTransactions.get(i);

		while (!helpedTransaction->isInitial() && !helpedTransaction->isCommited()) {
			Thread::yield();
		}

		info("helping transaction " + helpedTransaction->getLoggingName());

		if (!helpedTransaction->start())
			continue;

		helpedTransaction->commit();
	}
}

void Transaction::reset() {
	info("reset");

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
	info("acquiring read/write objects");

	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readWriteObjects.get(i);

		if (handle->hasObjectChanged()) {
			info("object has changed on acquiring RW objects");
			return false;
		}

		while (!handle->acquireHeader(this)) {
			Transaction* competingTransaction = handle->getCompetingTransaction();
			if (competingTransaction == NULL) {
				info("conflict with already released transaction");
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

	info("finished acquiring read/write objects");

	return true;
}

void Transaction::releaseReadWriteObjects() {
	info("releasing read/write objects");

	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readWriteObjects.get(i);

		handle->releaseHeader();
	}

	info("finished releasing read/write objects");
}

bool Transaction::validateReadOnlyObjects() {
	info("validating read only objects");

	for (int i = 0; i < readOnlyObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readOnlyObjects.get(i);

		if (handle->hasObjectChanged()) {
			info("object has changed on validating RO objects");
			return false;
		}

		Transaction* competitorTransaction = handle->getCompetingTransaction();

		if (competitorTransaction != NULL && competitorTransaction->isReadChecking()) {
			if (!resolveConflict(competitorTransaction))
				return false;
		}
	}

	info("finished validating read only objects");

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
		info("aborting conflicting transction " + transaction->getLoggingName());

		helpTransaction(transaction);

		return transaction->isAborted();
	} else {
		info("aborting self due to conflict");

		//transaction->helpTransaction(this);

		return false;
	}
}

void Transaction::helpTransaction(Transaction* transaction) {
	transaction->abort();

	if (transaction->isAborted()) {
		if (!transaction->setHelperTransaction((this)))
			return;

		info("adding helped transaction " + transaction->getLoggingName());

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
