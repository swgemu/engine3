/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/core/Core.h"

#include "TransactionalMemoryManager.h"

using namespace engine::stm;

class PureTask : public Task {
public:
	void run() {
		//Logger::console.warning("pure transaction called");
	}
};

class TransactionalMemoryManagerStatisticsTask : public Thread {
public:
	void run() {
		//Thread::sleep(1000);

		while (true) {
			TransactionalMemoryManager::instance()->printStatistics();

			Thread::sleep(1000);
		}
	}
};

//Vector<bool> commitedTrans;
AtomicBoolean initializationTransactionStarted;

TransactionalMemoryManager::TransactionalMemoryManager() : Logger("TransactionalMemoryManager"), heapLock(true) {
	setInstance(this);

#ifdef MEMORY_PROTECTION
	objectHeap.create(2024UL * 1024UL * 1024UL);
	objectHeap.protect();
#endif

	taskManager = (TransactionalTaskManager*) Core::getTaskManager();

	objectManager = (TransactionalObjectManager*) Core::getObjectBroker();

	socketManager = new TransactionalSocketManager();

	baseClientManager = new TransactionalBaseClientManager();

	Thread* task = new TransactionalMemoryManagerStatisticsTask();
	task->start();

	/*for (int i = 0; i < 101000; ++i)
		commitedTrans.add(false);*/

	initializationTransactionStarted = false;

	reclaiming = false;

	setInfoLogLevel();
	setGlobalLogging(true);
}

TransactionalMemoryManager::~TransactionalMemoryManager() {
	/*printf("Undone transactions: ");

	for (unsigned int i = 1; i < transactionID.get(); ++i) {
		if (!commitedTrans.get(i))
			printf("%i ", i);
	}

	printf("\n");*/

	setInstance(nullptr);
}

void TransactionalMemoryManager::commitPureTransaction(Transaction* transaction) {
	Reference<Task*> task = new PureTask();

	Reference<engine::stm::Transaction*> trans = transaction;

	if (!transaction->start(task) || !transaction->commit())
		assert(0 && "pure transaction failed");
}

Transaction* TransactionalMemoryManager::getTransaction() {
	Reference<Transaction*>* transaction = currentTransaction.get();

	if (transaction == nullptr) {
		//transaction = new Transaction(transactionID.increment());
		transaction = new Reference<Transaction*>();
		currentTransaction.set(transaction);
	}

	return *transaction;
}

void TransactionalMemoryManager::setCurrentTransaction(Transaction* transaction) {
	Reference<Transaction*>* trans = currentTransaction.get();

	if (trans == nullptr) {
		//transaction = new Transaction(transactionID.increment());
		trans = new Reference<Transaction*>();
		currentTransaction.set(trans);
	}

	*trans = transaction;
	//currentTransaction.set(transaction);
}

Transaction* TransactionalMemoryManager::startTransaction() {
	//blockLock.rlock();

	Reference<Transaction*>* current = currentTransaction.get();

	if (current == nullptr) {
		current = new Reference<Transaction*>();
		currentTransaction.set(current);
	}

	assert(*current == nullptr);

	if (transactionID > 0) {
		while (!initializationTransactionStarted) {
			Thread::sleep(500);

			Thread::yield();
		}
	}

	//currentTransaction.set(transaction);
	*current = new Transaction(transactionID.increment());

	startedTransactions.increment();

	return *current;
}

void TransactionalMemoryManager::commitTransaction() {
	Reference<Transaction*>* transaction = currentTransaction.get();
	//commitedTrans.set(transaction->getIdentifier(), true);

	//debug("Executing tasks: " + String::valueOf(Core::getTaskManager()->getExecutingTaskSize()));
	Vector<Object*>* objects = getReclamationList();
	//debug("Reclamation list size:" + String::valueOf(objects->size()));

	reclaimObjects(5000, 0);

	commitedTransactions.increment();
	totalCommitedTransactions.increment();

	if ((*transaction)->getIdentifier() == 1)
		initializationTransactionStarted.compareAndSet(false, true);

	//currentTransaction.set(nullptr);
	*transaction = nullptr;

	//blockLock.runlock();
}

void TransactionalMemoryManager::abortTransaction() {
//	blockLock.runlock();

	Reference<Transaction*>* transaction = currentTransaction.get();

	//reclaimObjects(0, 0);

	//currentTransaction.set(nullptr);
	*transaction = nullptr;

	abortedTransactions.increment();
}

void TransactionalMemoryManager::retryTransaction() {
	retryConflicts.increment();
}

void TransactionalMemoryManager::reclaim(Object* object) {
	/*if (!object->_setDestroying())
		return;*/

	Reference<Transaction*> transaction = getTransaction();

	if (transaction == nullptr) {
		//we are out of transaction... transaction probably commited so we are deleting the transaction
		destroy(object);
	} else {
		transaction->deleteObject(object);
	}
}

Object* TransactionalMemoryManager::create(size_t size) {
#ifdef MEMORY_PROTECTION
	return (Object*) objectHeap.allocate(size);
#else
	return (Object*) ::malloc(size);
#endif
}

void TransactionalMemoryManager::destroy(Object* object) {
#ifdef MEMORY_PROTECTION
	if (objectHeap.contains(object)) {
		object->~Object();
		//debug("object " + object->toString() + " was destroyed");

		objectHeap.free(object);
	} else
#endif
		delete object;
}

void TransactionalMemoryManager::reclaimObjects(int objectsToSpare, int maxObjectsToReclaim) {
	reclaiming = true;

	Vector<Object*>* objects = getReclamationList();

	int objectsReclaimed = 0;

	uint64 startTime = System::getMikroTime();

	//while (objects->size() > objectsToSpare) {
	KernelCall kernelCall;

	/*if (objects->size() > 50000) {
		info("clearing objects", true);*/
		for (int i = 0; i < objects->size(); ++i) {
			Object* obj = objects->get(i);

			if (obj->getReferenceCount() == 0) {
				destroy(obj);

				/*if (++objectsReclaimed == maxObjectsToReclaim)
				break;*/
			} else {
				//obj->_clearDestroying();

				//deleted.drop((uint64)obj);
			}
		}

		objects->removeAll(51000, 10000);
	//}

	//if (objectsReclaimed > 0)
	/*	debug(String::valueOf(objectsReclaimed) + " objects were reclaimed in "
				+ String::valueOf(System::getMikroTime() - startTime) + " Us, "
				+ String::valueOf(objects->size()) + " remained in queue");*/

	Reference<Transaction*> transaction = getTransaction();
	Vector<Object*>* objectsForNextCommit = transaction->getDeletedObjects();

	if (objectsForNextCommit->size() > 0) {
		//objects->removeAll(objectsForNextCommit->size(), 100);

		objects->addAll(*objectsForNextCommit);

		/*for (int i = 0; i < objectsForNextCommit->size(); ++i) {
		Object* obj = objectsForNextCommit->get(i);

		objects->add(obj);
	}*/
	}

	objectsForNextCommit->removeAll();

	reclaiming = false;
}

Vector<Object*>* TransactionalMemoryManager::getReclamationList() {
	Vector<Object*>* objects = reclamationList.get();

	if (objects == nullptr) {
		objects = new Vector<Object*>(50000, 10000);

		reclamationList.set(objects);
	}

	return objects;
}

void TransactionalMemoryManager::setKernelMode() {
#ifdef MEMORY_PROTECTION
	heapLock.lock();

	objectHeap.unprotect();
#endif
}

void TransactionalMemoryManager::setUserMode() {
#ifdef MEMORY_PROTECTION
	objectHeap.protect();

	heapLock.unlock();
#endif
}

void TransactionalMemoryManager::printStatistics() {
	if (startedTransactions.get() == 0)
		return;

	TaskManager* taskManager = Core::getTaskManager();

	StringBuffer str;
	str << "transactions(started " << startedTransactions.get() << ", "
			<< "commited " << commitedTransactions.get() << ", "
			<< "totalCommited " << totalCommitedTransactions.get() << ", "
			<< "retried " << retryConflicts.get() << ", "
			<< "deleted " << deletedTransactions.get() << ", "
			<< "aborted " << abortedTransactions.get() << ") - tasks ("
			<< "exectuing " << taskManager->getExecutingTaskSize() << ", "
			<< "scheduled " << taskManager->getScheduledTaskSize() << ") "
			/*<< "created handles " << HandleCounter::createdHandles.get() << ", "
			<< "deleted handles " << HandleCounter::deletedHandles.get() << ", "*/
			<< "aborted due exceptions " << failedToExceptions << ", "
			<< "due to object changed " << failedOnAcquireRW << ", "
			<< "due to commited competing transaction " << failedCompetingCommited;


	info(str);

	startedTransactions.set(0);
	commitedTransactions.set(0);
	abortedTransactions.set(0);
	retryConflicts.set(0);
	failedToExceptions.set(0);
	failedOnAcquireRW.set(0);
	failedCompetingCommited.set(0);
	deletedTransactions.set(0);
}
