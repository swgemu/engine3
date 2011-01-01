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

	String threadName = Thread::getCurrentThread()->getName();

	int tid = transactionID.increment();

	setLoggingName("Transaction " + String::valueOf(tid) + "(" + threadName + ")");
	setLogging(false);
	setGlobalLogging(true);
}

Transaction::~Transaction() {
	reset();
}

void Transaction::start() {
	start(task);
}

void Transaction::start(Runnable* task) {
	assert(task != NULL && "task was NULL");

	Transaction::task = task;

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

		TransactionalMemoryManager::instance()->clearTransaction();
	} else {
		info("aborted");

		++commitAttempts;

		reset();
	}

	for (int i = 0; i < helpedTransactions.size(); ++i) {
		Transaction* helpedTransaction = helpedTransactions.get(i);

		info("helping transaction " + helpedTransaction->getLoggingName());

		helpedTransaction->start();

		if (helpedTransaction->commit())
			delete helpedTransaction;
	}

	return commited;
}

bool Transaction::doCommit() {
	if (!isUndecided()) {
		doAbort();
		return false;
	}

	if (!acquireReadWriteObjects())
		return false;

	if (!setState(UNDECIDED, READ_CHECKING)) {
		doAbort();
		return false;
	}

	if (!validateReadOnlyObjects())
		return false;

	if (!setState(READ_CHECKING, COMMITTED)) {
		doAbort();
		return false;
	}

	releaseReadWriteObjects();

	for (int i = 0; i < commands.size(); ++i) {
		Command* command = commands.get(i);

		command->execute();
	}

	return true;
}

void Transaction::abort() {
	info("aborting");

	setState(ABORTED);
}

void Transaction::doAbort() {
	info("doing abort");

	status = ABORTED;

	discardReadWriteObjects();

	for (int i = 0; i < commands.size(); ++i) {
		Command* command = commands.get(i);

		command->undo();
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

	info("reset");
}

bool Transaction::acquireReadWriteObjects() {
	info("acquiring read/write objects");

	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readWriteObjects.get(i);

		if (handle->hasObjectChanged()) {
			doAbort();
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
			doAbort();
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
		helpTransaction(transaction);

		return true;
	} else {
		this->doAbort();

		return false;
	}
}

void Transaction::helpTransaction(Transaction* transaction) {
	helpedTransactions.add(transaction);

	transaction->abort();
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
