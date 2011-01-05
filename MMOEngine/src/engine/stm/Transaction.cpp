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

AtomicInteger transactionID;

Transaction::Transaction() : Logger() {
	status = UNDECIDED;

	commitTime = 0;
	commitAttempts = 1;

	task = NULL;

	Command* command = (TransactionalTaskManager*) Core::getTaskManager();
	commands.add(command);

	command = TransactionalMemoryManager::instance()->getObjectManager();
	commands.add(command);

	command = TransactionalMemoryManager::instance()->getSocketManager();
	commands.add(command);


	tid = transactionID.increment();

	setLogging(false);
	setGlobalLogging(true);
}

Transaction::~Transaction() {
	reset();

	info("deleted");
}

void Transaction::start() {
	start(task);
}

void Transaction::start(Runnable* task) {
	assert(task != NULL && "task was NULL");

	Transaction::task = task;

	String threadName = Thread::getCurrentThread()->getName();

	setLoggingName("Transaction " + String::valueOf(tid) + "(" + threadName + ")");

	info("starting transaction");

	TransactionalMemoryManager::instance()->setTransaction(this);

	task->run();
}

bool Transaction::commit() {
	info("commiting..");

	uint64 startTime = System::getMikroTime();

	int readOnlyObjectsCount = readOnlyObjects.size();
	int readWriteObjectsCount = readWriteObjects.size();

	bool commited = doCommit();

	commitTime += System::getMikroTime() - startTime;

	if (commited) {
		info("commited (" + String::valueOf(commitTime) + "Us, "
				+ String::valueOf(commitAttempts) + " tries, R/W objects "
				+readOnlyObjectsCount + "/" + readWriteObjectsCount +")");

		doHelpTransactions();
	} else {
		info("aborted");

		++commitAttempts;

		if (helperTransaction.get() == NULL && helperTransaction.compareAndSet(NULL, this)) {
			reset();

			info("helping self");

			Thread::yield();

			start();

			return commit();
		}
	}

	Thread::yield();

	return commited;
}

bool Transaction::doCommit() {
	if (!isUndecided()) {
		doAbort("transaction not in UNDECIDED state on commit");
		return false;
	}

	if (!acquireReadWriteObjects())
		return false;

	if (!setState(UNDECIDED, READ_CHECKING)) {
		doAbort("status change UNDECIED -> READ CHECKING failed");
		return false;
	}

	if (!validateReadOnlyObjects())
		return false;

	if (!setState(READ_CHECKING, COMMITTED)) {
		doAbort("status change READ CHECKING -> COMMITED failed");
		return false;
	}

	releaseReadWriteObjects();

	for (int i = 0; i < commands.size(); ++i) {
		Command* command = commands.get(i);

		command->execute();
	}

	TransactionalMemoryManager::instance()->clearTransaction();

	return true;
}

void Transaction::abort() {
	setState(ABORTED);

	if (status == ABORTED)
		info("aborting");
	else
		info("aborting failed");
}

void Transaction::doAbort(const char* reason) {
	info("doing abort: " + String(reason));

	status = ABORTED;

	discardReadWriteObjects();

	for (int i = 0; i < commands.size(); ++i) {
		Command* command = commands.get(i);

		command->undo();
	}

	reset();
}

void Transaction::doHelpTransactions() {
	for (int i = 0; i < helpedTransactions.size(); ++i) {
		Reference<Transaction*> helpedTransaction = helpedTransactions.get(i);

		if (helpedTransaction->isUndecided()) {
			info("helping transaction " + helpedTransaction->getLoggingName());

			helpedTransaction->start();

			helpedTransaction->commit();
		} else {
			helpedTransaction->clearHelperTransaction();
		}
	}
}

void Transaction::reset() {
	status = UNDECIDED;

	openedObjets.removeAll();

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

	TransactionalMemoryManager::instance()->clearTransaction();

	info("reset");
}

bool Transaction::acquireReadWriteObjects() {
	info("acquiring read/write objects");

	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readWriteObjects.get(i);

		if (handle->hasObjectChanged()) {
			doAbort("object has changed on acquiring RW objects");
			return false;
		}

		if (!handle->acquireHeader(this)) {
			Transaction* competingTransaction = handle->getCompetingTransaction();

			if (!resolveConflict(competingTransaction))
				return false;
		}
	}

	for (int i = 0; i < readOnlyObjects.size(); ++i) {
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
	}

	info("finished acquiring read/write objects");

	return true;
}

void Transaction::releaseReadWriteObjects() {
	info("releasing read/write objects");

	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readWriteObjects.get(i);
		handle->releaseHeader();

		delete handle;
	}

	readWriteObjects.removeAll();

	info("finished releasing read/write objects");
}

bool Transaction::validateReadOnlyObjects() {
	info("validating read only objects");

	for (int i = 0; i < readOnlyObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readOnlyObjects.get(i);

		if (handle->hasObjectChanged()) {
			doAbort("object has changed on validating RO objects");
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
		info("abrting conflicting transction " + transaction->getLoggingName());

		helpTransaction(transaction);

		return true;
	} else {
		info("aborting self due to conflict");

		transaction->helpTransaction(this);

		this->doAbort("conflict with other transaction");

		return false;
	}
}

void Transaction::helpTransaction(Transaction* transaction) {
	if (!transaction->setHelperTransaction((this)))
		return;

	transaction->abort();

	if (transaction->isAborted()) {
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
