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

ReadWriteLock Transaction::blockLock;

Transaction::Transaction(int id) : Logger() {
	status = INITIAL;

	commitTime = 0;
	runTime = 0;
	commitAttempts = 1;

	task = NULL;

	helperTransaction = NULL;

	Command* command = TransactionalMemoryManager::instance()->getObjectManager();
	commands.add(command);

	command = (TransactionalTaskManager*) Core::getTaskManager();
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
		error("failed to start transaction (state " + String::valueOf(status) + ") per:" + String::valueOf(task->getPeriod()));

		return false;
	}

	assert(task != NULL && "task was NULL");

	Transaction::task = task;

	//helperTransaction = NULL;

	String threadName = Thread::getCurrentThread()->getName();
	setLoggingName("Transaction " + String::valueOf(tid) + "(" + threadName + ")");

	TransactionalMemoryManager::instance()->startTransaction(this);

	uint64 startTime = System::getMikroTime();

	try {
		debug("starting transaction");

		task->run();
	} catch (TransactionAbortedException& e) {
		abort();
		TransactionalMemoryManager::instance()->increaseFailedByExceptions();
	} catch (Exception& e) {
		e.printStackTrace();

		TransactionalMemoryManager::instance()->increaseFailedByExceptions();

		abort();
	}

	runTime += System::getMikroTime() - startTime;

	blockLock.rlock();

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
		String msg = "ran and commited in " + String::valueOf((commitTime + runTime) / 1000) + "ms Task: " + String(TypeInfo<Task>::getClassName(task)) + " commited (" + String::valueOf(runTime) + "Us / " + String::valueOf(commitTime) + "Us, "
				+ String::valueOf(commitAttempts) + " tries, R/W objects "
				+ readOnlyObjectsCount + " / " + readWriteObjectsCount +")";

		/*if (commitTime + runTime > 50000)
			warning(msg);
		else*/
			debug(msg);


		doHelpTransactions();
	} else {
		doAbort();

		if (helperTransaction.compareAndSet(NULL, this)) {
			Thread::yield();

			debug("helping self");

			commited = doRetry(this);
		}
	}

	//TransactionalMemoryManager::instance()->setCurrentTransaction(NULL);

	Thread::yield();

	blockLock.runlock();

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

		try {
			command->execute();
		} catch (TransactionAbortedException& e) {
			return false;
		} catch (Exception& e) {
			e.printStackTrace();
		}
	}

	releaseReadWriteObjects();

	//outside releaseRWO for perfs to not block transactions since this ops are async
	try {
		Command* command = TransactionalMemoryManager::instance()->getBaseClientManager();
		command->execute();

		command = TransactionalMemoryManager::instance()->getSocketManager();
		command->execute();
	} catch (TransactionAbortedException& e) {
		error("TransactionAbortedException after releasingRWObjects");
	} catch (Exception& e) {
		e.printStackTrace();
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
		debug("aborting");
	else
		debug("aborting failed");
}

void Transaction::doAbort() {
	Time startTime;

	status = ABORTED;

	discardReadWriteObjects();

	for (int i = 0; i < commands.size(); ++i) {
		Command* command = commands.get(i);

		command->undo();
	}


	Command* command = TransactionalMemoryManager::instance()->getBaseClientManager();
	command->undo();

	command = TransactionalMemoryManager::instance()->getSocketManager();
	command->undo();

	debug("aborted (" + String::valueOf(runTime) + "Us / " + String::valueOf(commitTime) + "Us, "
			+ String::valueOf(commitAttempts) + " tries, R/W objects "
			+ readOnlyObjects.size() + " / " + readWriteObjects.size() +")");

	if (commitAttempts > 1)
		TransactionalMemoryManager::instance()->retryTransaction();

	++commitAttempts;

	reset();

	TransactionalMemoryManager::instance()->abortTransaction();

	/*int64 mili = startTime.miliDifference();

	if (mili > 30) {
		String msg = "aborted in " + String::valueOf(mili) + "ms Task: " + String(TypeInfo<Task>::getClassName(task)) + " aborted (" + String::valueOf(runTime) + "Us / " + String::valueOf(commitTime) + "Us, ";

		warning(msg);
	}*/
}

bool Transaction::doRetry(Transaction* helper) {
	//TransactionalMemoryManager::instance()->getTaskManager()->getTaskManagerImpl()->executeTask(task);

	while (!isCommited()) {
		Thread::yield();

		if (start()) {

			commit();
		}
	}

	/*if () {
		while (!isCommited())
		setHelperTransaction(helper);

		return commit();
	}*/

	return true;

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

		/*if (i > 1000)
			warning("too much waiting? " + String::valueOf(i));*/

		debug("helping transaction " + helpedTransaction->getLoggingName());

		helpedTransaction->doRetry(this);
	}

//	TransactionalMemoryManager::instance()->setCurrentTransaction(this);

	helpedTransactions.removeAll();
}

void Transaction::reset() {
	debug("reset");

	openedObjets.removeAll();

	localObjectCache.removeAll();

	/*SortedVector<uint64> deleted;
	deleted.setNoDuplicateInsertPlan();*/

	for (int i = 0; i < readOnlyObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readOnlyObjects.get(i);

		//if (deleted.put((uint64)handle) != -1)
			delete handle;
	}

	readOnlyObjects.removeAll();

	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readWriteObjects.get(i);

		//if (deleted.put((uint64)handle) != -1)
			delete handle;

	}

	readWriteObjects.removeAll();

	helpedTransactions.removeAll();

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
				/*debug("conflict with already released transaction");
				return false;*/
				Thread::yield();
				continue;
			}

			if (competingTransaction->compareTo(this) < 0)
				resolveConflict(competingTransaction);
			else
				return false;


			/*if (!resolveConflict(competingTransaction)) {
				debug("could not resolve conflict");
				return false;
			}*/

			Thread::yield();
		}

		if (handle->hasObjectChanged()) {
			debug("object '" + handle->getObjectLocalCopy()->toString() + "' has changed on acquiring RW objects");
			return false;
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

	//Vector<Reference<Object*> > objects(100, 100);

	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandle<Object*>* handle = readWriteObjects.get(i);

		/*Object* objectCopy = handle->getObjectLocalCopy();

		if (objectCopy != NULL) {
			objects.add(objectCopy);
		}*/

		handle->releaseHeader();
	}

	//TransactionalMemoryManager::instance()->getObjectManager()->addObjectsToSave(objects);

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

void Transaction::deleteObject(Object* object) {
	reclaimedObjects.add(object);
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
