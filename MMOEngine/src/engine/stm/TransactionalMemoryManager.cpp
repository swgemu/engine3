/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/core/Core.h"

#include "TransactionalMemoryManager.h"
#include "Transaction.h"

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
		Thread::sleep(1000);

		while (true) {
			TransactionalMemoryManager::instance()->printStatistics();

			Thread::sleep(1000);
		}
	}
};

//Vector<bool> commitedTrans;
AtomicBoolean initializationTransactionStarted;

TransactionalMemoryManager::TransactionalMemoryManager() : Logger("TransactionalMemoryManager") {
	setInstance(this);

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

	setInstance(NULL);
}

void TransactionalMemoryManager::commitPureTransaction() {
	Reference<Task*> task = new PureTask();

	task->execute();
}

Transaction* TransactionalMemoryManager::getTransaction() {
	Reference<Transaction*>* transaction = currentTransaction.get();

	if (transaction == NULL) {
		//transaction = new Transaction(transactionID.increment());
		transaction = new Reference<Transaction*>();
		currentTransaction.set(transaction);
	}

	if (*transaction == NULL) {
		*transaction = new Transaction(transactionID.increment());

		//transaction->info("created");
	}

	return *transaction;
}

void TransactionalMemoryManager::setCurrentTransaction(Transaction* transaction) {
	Reference<Transaction*>* trans = currentTransaction.get();

	*trans = transaction;
	//currentTransaction.set(transaction);
}

void TransactionalMemoryManager::startTransaction(Transaction* transaction) {
	//blockLock.rlock();

	Reference<Transaction*>* current = currentTransaction.get();

/*	while (current != NULL) {
		current = currentTransaction.get();

		Thread::sleep(10);
	}*/

	/*while (commitedTrans.size() < transaction->getIdentifier())
		commitedTrans.add(false);*/

	//commitedTrans.add(transaction->getIdentifier(), false);

	if (transaction->getIdentifier() != 1) {
		while (!initializationTransactionStarted.get()) {
			Thread::sleep(1000);

			*current = *currentTransaction.get();
		}
	}


	assert(*current == NULL || *current == transaction);

	//currentTransaction.set(transaction);
	*current = transaction;

	startedTransactions.increment();
}

void TransactionalMemoryManager::commitTransaction() {
	Reference<Transaction*>* transaction = currentTransaction.get();
	//commitedTrans.set(transaction->getIdentifier(), true);

	debug("Executing tasks: " + String::valueOf(Core::getTaskManager()->getExecutingTaskSize()));
	Vector<Object*>* objects = getReclamationList();
	debug("Reclamation list size:" + String::valueOf(objects->size()));

	reclaimObjects(0, 0);

	commitedTransactions.increment();

	if ((*transaction)->getIdentifier() == 1)
		initializationTransactionStarted.compareAndSet(false, true);

	//currentTransaction.set(NULL);
	*transaction = NULL;

	//blockLock.runlock();
}

void TransactionalMemoryManager::abortTransaction() {
//	blockLock.runlock();

	Reference<Transaction*>* transaction = currentTransaction.get();

	//reclaimObjects(0, 0);

	//currentTransaction.set(NULL);
	*transaction = NULL;

	abortedTransactions.increment();
}

void TransactionalMemoryManager::retryTransaction() {
	retryConflicts.increment();
}

void TransactionalMemoryManager::reclaim(Object* object) {
	if (!object->_setDestroying())
		return;

	/*Transaction* trans = dynamic_cast<Transaction*>(object);

	if (trans != NULL)  {
		trans->destroy();
		return;
	}*/

	Reference<Transaction*> transaction = Transaction::currentTransaction();
	transaction->deleteObject(object);
}

void TransactionalMemoryManager::create(Object* object) {
	/*Locker locker(&deletedMutex);

	int find = deleted.find((uint64)object);

	if (find != -1) {
		delete deleted.get(find);
		deleted.remove(find);
	}*/

}

void TransactionalMemoryManager::reclaimObjects(int objectsToSpare, int maxObjectsToReclaim) {
	reclaiming = true;

	Vector<Object*>* objects = getReclamationList();

	int objectsReclaimed = 0;

	uint64 startTime = System::getMikroTime();

	//Locker locker(&deletedMutex);

	/*if (deleted.contains((uint64)object)) {
		error("Object already on deleted list");
		DeletedTrace trace;
		trace.print();

		error("previously reclaimed here");
		deleted.get((uint64)object)->print();
	} else
		deleted.put((uint64)object, new DeletedTrace());*/

	//while (objects->size() > objectsToSpare) {
	for (int i = 0; i < objects->size(); ++i) {
		Object* obj = objects->get(i);

		if (obj->getReferenceCount() == 0) {
			MemoryManager::reclaim(obj);

			/*if (deleted.contains((uint64)obj)) {
				error("Object already on deleted list");
				DeletedTrace trace;
				trace.setTid(currentTransaction.get()->getIdentifier());
				trace.print();

				error("previously reclaimed here");
				deleted.get((uint64)obj)->print();
			} else {
				DeletedTrace* trace = new DeletedTrace();
				trace->setTid(currentTransaction.get()->getIdentifier());
				deleted.put((uint64)obj, trace);
			}*/

			if (++objectsReclaimed == maxObjectsToReclaim)
				break;
		} else {
			obj->_clearDestroying();

			//deleted.drop((uint64)obj);
		}
	}



	debug(String::valueOf(objectsReclaimed) + " objects were reclaimed in "
			+ String::valueOf(System::getMikroTime() - startTime) + " Us, "
			+ String::valueOf(objects->size()) + " remained in queue");

	Reference<Transaction*> transaction = Transaction::currentTransaction();
	Vector<Object*>* objectsForNextCommit = transaction->getDeletedObjects();

	if (objectsForNextCommit->size() > 0) {
		objects->removeAll(objectsForNextCommit->size(), 100);

		objects->addAll(*objectsForNextCommit);

		/*for (int i = 0; i < objectsForNextCommit->size(); ++i) {
		Object* obj = objectsForNextCommit->get(i);

		objects->add(obj);
	}*/
	} else
		objects->removeAll(100, 100);

	objectsForNextCommit->removeAll();

	reclaiming = false;
}

Vector<Object*>* TransactionalMemoryManager::getReclamationList() {
	Vector<Object*>* objects = reclamationList.get();

	if (objects == NULL) {
		objects = new Vector<Object*>();

		reclamationList.set(objects);
	}

	return objects;
 }

void TransactionalMemoryManager::printStatistics() {
	if (startedTransactions.get() == 0)
		return;

	TaskManager* taskManager = Core::getTaskManager();

	StringBuffer str;
	str << "transactions(started " << startedTransactions.get() << ", "
			<< "commited " << commitedTransactions.get() << ", "
			<< "retried " << retryConflicts.get() << ", "
			<< "aborted " << abortedTransactions.get() << ") - tasks ("
			<< "exectuing " << taskManager->getExecutingTaskSize() << ", "
			<< "scheduled " << taskManager->getScheduledTaskSize() << ") "
			<< "aborted due exceptions " << failedToExceptions << ", "
			<< "due to object changed " << failedOnAcquireRW;


	info(str);

	startedTransactions.set(0);
	commitedTransactions.set(0);
	abortedTransactions.set(0);
	retryConflicts.set(0);
	failedToExceptions.set(0);
	failedOnAcquireRW.set(0);
}
