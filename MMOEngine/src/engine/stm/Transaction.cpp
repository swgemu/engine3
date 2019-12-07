/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/core/Core.h"

#include "TransactionalMemoryManager.h"

#include "algos/FraserSTM.h"

using namespace engine::stm;

ReadWriteLock Transaction::blockLock;

AtomicInteger HandleCounter::createdHandles;
AtomicInteger HandleCounter::deletedHandles;

volatile int ArrayCloneCounter::totalCount = 0;

UniqueReference<STMAlgorithm*> Transaction::commitAlgorithm(new FraserSTM());

Transaction::Transaction(uint64 id) : Logger(), task(nullptr) {
	status = INITIAL;

	commitTime = 0;
	runTime = 0;

	openedObjets.setNullValue(nullptr);

	Command* command = TransactionalMemoryManager::instance()->getObjectManager();
	commands.add(command);

	command = (TransactionalTaskManager*) Core::getTaskManager();
	commands.add(command);

	command = TransactionalMemoryManager::instance()->getBaseClientManager();
	commands.add(command);

	command = TransactionalMemoryManager::instance()->getSocketManager();
	commands.add(command);

	tid = id;

	setInfoLogLevel();
	setGlobalLogging(true);
}

Transaction::~Transaction() {
	//debug("deleted");

	TransactionalMemoryManager::instance()->increaseDeletedTransactions();
}

bool Transaction::start() {
	return start(task);
}

bool Transaction::start(Task* task) NO_THREAD_SAFETY_ANALYSIS {
	fatal(setState(INITIAL, UNDECIDED)) << "could not move to undecided";
	fatal(task != nullptr) << "task is null";

	Transaction::task = task;

	bool successFullStart = true;

	String threadName = Thread::getCurrentThread()->getName();
	setLoggingName("Transaction " + String::valueOf(tid) + "(" + threadName + ")");

	uint64 startTime = System::getMikroTime();

	assert(isUndecided());

	try {
		//debug("starting transaction");

		task->run();
	} catch (TransactionAbortedException& e) {
		//successFullStart = false;

		status = ABORTED;

		TransactionalMemoryManager::instance()->increaseFailedByExceptions();

		doAbort();

		//error("ebati v rot");
	} catch (Exception& e) {
		error("exception running a task " + e.getMessage());
		e.printStackTrace();
	}

	runTime += System::getMikroTime() - startTime;

	blockLock.rlock();

	return successFullStart;
}

bool Transaction::commit() NO_THREAD_SAFETY_ANALYSIS {
	//debug("commiting..");

	uint64 startTime = System::getMikroTime();

	int readOnlyObjectsCount = readOnlyObjects.size();
	int readWriteObjectsCount = readWriteObjects.size();

	//info("Commiting with readOnly:" + String::valueOf(readOnlyObjectsCount) + " and readWrite:" + String::valueOf(readWriteObjectsCount), true);

	bool commited = doCommit();

	if (commited) {
		finishCommit();

		commitTime += System::getMikroTime() - startTime;

		/*String msg = "ran and commited in " +
		  String::valueOf((commitTime + runTime) / 1000) + "ms Task: " + String(TypeInfo<Object>::getClassName(task)) +
		  " commited (" + String::valueOf(runTime) + "Us / " + String::valueOf(commitTime) + "Us, "
				+ String::valueOf(commitAttempts) + " tries, R/W objects "
				+ readOnlyObjectsCount + " / " + readWriteObjectsCount +")";*/
	} else {
		doAbort();
	}

	this->task = nullptr;

	Thread::yield();

	blockLock.runlock();

	return commited;
}

void Transaction::finishCommit() {
	for (int i = 0; i < commands.size(); ++i) {
		Command* command = commands.get(i);

		try {
			command->execute();
		} catch (TransactionAbortedException& e) {
			//return false;
			error("TransactionAbortedException while executing commands isnt allowed");
		} catch (Exception& e) {
			e.printStackTrace();
		}
	}

	TransactionalMemoryManager::instance()->commitTransaction();

	//openedObjets.removeAll();
	localObjectCache.removeAll();
}

void Transaction::resolveAbortedHandles() {
/*	for (int i = 0; i < readWriteObjects.size(); ++i) {
		Reference<TransactionalObjectHandle<Object*>*> handle = readWriteObjects.get(i);

		Reference<TransactionalObjectHandle<Object*>*> last = handle->getLastHandle();

		Reference<TransactionalObjectHandle<Object*>*> lastRef = last;

		while (last != nullptr) {
			Transaction* transaction = last->getTransaction();

			if (transaction != this && transaction != nullptr) {
				transaction->abort();

				if (transaction->isAborted()) {
					if (transaction->setHelperTransaction(this)) {
						//warning("adding helped transaction god help us");

						helpedTransactions.put(transaction);
					}
				}
			}

			last->setTransaction(nullptr);

			lastRef = last;

			last = last->getPrevious();

			lastRef->setPrevious(nullptr);
		}

		//while (last->getPrevious())

		//iterate conflicting transactions

	}*/
}

void Transaction::cleanReadOnlyHandles() {
	//for (int i = 0; i <)
	//int i;
	//while ((i = currentReadOnlyHandleCleaning.increment()) < readOnlyObjects.size()) {
	/*for (int i = 0; i < readOnlyObjects.size(); ++i) {
		Reference<TransactionalObjectHandle<Object*>* > handle = readOnlyObjects.get(i);

		handle->setTransaction(nullptr);
	}*/
}

bool Transaction::doCommit() {
	return commitAlgorithm->doCommit(this);
}

void Transaction::doAbort() {
	Time startTime;

	assert(isAborted());

	for (int i = 0; i < commands.size(); ++i) {
		Command* command = commands.get(i);

		command->undo();
	}

	/*debug("aborted (" + String::valueOf(runTime) + "Us / " + String::valueOf(commitTime) + "Us, tries, R/W objects "
			+ readOnlyObjects.size() + " / " + readWriteObjects.size() +")");*/

	TransactionalMemoryManager::instance()->reclaimObjects(0, 0);

	//openedObjets.removeAll();

	localObjectCache.removeAll();

	//We cant clear RW o RO vectors because other threads might be still trying to help us!

	TransactionalMemoryManager::instance()->abortTransaction();
}

void Transaction::releaseReadWriteObjects() {
	//debug("releasing read/write objects");

	//Warning! This can be called by several threads concurrently

	KernelCall kernelCall;

	//while ((i = currentReadWriteObjectCleanup.increment()) < readWriteObjects.size()) {
	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandleBase* handle = readWriteObjects.get(i);

		handle->releaseHeader();
	}

	//TransactionalMemoryManager::instance()->getObjectManager()->addObjectsToSave(objects);

	//debug("finished releasing read/write objects");
}

void Transaction::discardReadWriteObjects() {
	//while ((i = currentReadWriteObjectCleanup.increment()) < readWriteObjects.size()) {
	for (int i = 0; i < readWriteObjects.size(); ++i) {
		TransactionalObjectHandleBase* handle = readWriteObjects.get(i);

		handle->discardHeader(this);
	}
}

int Transaction::compareTo(Transaction* transaction) {
	if (transaction->tid > tid) {
		return 1;
	} else if (transaction->tid < tid)
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

String Transaction::toString() const {
	return "Transaction [" + Thread::getCurrentThread()->getName() + "] commited in " + Long::toString(commitTime) + " usec";
}

bool Transaction::setState(int newstate) {
	return setState(status.get(), newstate);
}

bool Transaction::setState(int currentstate, int newstate) {
	return status.compareAndSet(currentstate, newstate);
}
