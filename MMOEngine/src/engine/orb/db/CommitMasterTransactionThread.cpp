/*
 * CommitMasterTransactionThread.cpp
 *
 *  Created on: 18/02/2012
 *      Author: victor
 */

#include "engine/orb/DistributedObjectBroker.h"

#include "DOBObjectManager.h"

#include "UpdateModifiedObjectsThread.h"

#include "CommitMasterTransactionThread.h"

CommitMasterTransactionThread::CommitMasterTransactionThread() {
	transaction = nullptr;
	threads = nullptr;

	objectsToDeleteFromRam = nullptr;

	numberOfThreads = 0;

	doRun = true;
}

CommitMasterTransactionThread::~CommitMasterTransactionThread() {
	if (doRun) {
		shutdown();
	}
}

void CommitMasterTransactionThread::startWatch(engine::db::berkley::Transaction* trans, Vector<UpdateModifiedObjectsThread*>* workers, int number, Vector<DistributedObject* >* objectsToCollect) {
	assert(workers != nullptr);
	assert(objectsToCollect != nullptr);

	transaction = trans;
	threads = workers;
	numberOfThreads = number;
	objectsToDeleteFromRam = objectsToCollect;

	blockMutex.lock();

	waitCondition.signal(&blockMutex);

	blockMutex.unlock();
}

void CommitMasterTransactionThread::run() {
	while (doRun) {
		blockMutex.lock();

		waitCondition.wait(&blockMutex);

		if (doRun) {
			commitData();
		}

		transaction = nullptr;
		threads = nullptr;
		objectsToDeleteFromRam = nullptr;

		blockMutex.unlock();
	}
}

void CommitMasterTransactionThread::shutdown() {
	doRun = false;

	blockMutex.lock();

	waitCondition.broadcast(&blockMutex);

	blockMutex.unlock();

	join();
}

int CommitMasterTransactionThread::garbageCollect(DOBObjectManager* objectManager) {
	int i = 0;

	//      while (objectsToDeleteFromRam->size() != 0) {
	for (int j = 0; j < objectsToDeleteFromRam->size(); ++j) {
		DistributedObject* object = objectsToDeleteFromRam->getUnsafe(j);

		Locker locker(objectManager);

		//printf("object ref count:%d and updated flag:%d\n", object->getReferenceCount(), object->_isUpdated());

		if (object->getReferenceCount() == 2 && (!object->_isUpdated() || object->_isDeletedFromDatabase() || !object->isPersistent())) {
			if (objectManager->localObjectDirectory.tryRemoveHelper(object->_getObjectID())) {
				//localObjectDirectory.removeHelper(object->_getObjectID());

				++i;

				object = nullptr;
			}
		} /*else if (object->_isUpdated() && !object->_isDeletedFromDatabase()) {
			String text = TypeInfo<DistributedObject>::getClassName(object) + " 0x" + String::hexvalueOf((int64)object->_getObjectID());

			printf("%s refs:%d\n", text.toCharArray(), object->getReferenceCount());
		}*/

		if ((((j + 1) % 10000) == 0) || ((i + 1) % 100) == 0) {
			locker.release();

			Thread::sleep(25);
		}
	}

	delete objectsToDeleteFromRam;
	objectsToDeleteFromRam = nullptr;

	return i;
}

void CommitMasterTransactionThread::commitData() {
	for (int i = 0; i < numberOfThreads; ++i) {
		UpdateModifiedObjectsThread* worker = threads->get(i);

		while (!worker->hasFinishedCommiting()) {
			worker->signalMasterTransactionFinish();
			Thread::sleep(500);
		}
	}

	DOBObjectManager* objectManager = DistributedObjectBroker::instance()->getObjectManager();

	if (DistributedObjectBroker::instance()->isRootBroker()) {
		ObjectDatabaseManager::instance()->commitTransaction(transaction);
		ObjectDatabaseManager::instance()->checkpoint();

		objectManager->onCommitData();
	}

	objectManager->info("master transaction commited", true);

	int candidates = objectsToDeleteFromRam->size();
	objectManager->info("starting garbage collection for " + String::valueOf(candidates) + " candidates", true);

	int objs = garbageCollect(objectManager);

	objectManager->info("deleted from ram " + String::valueOf(objs) + " objects", true);

	objectManager->finishObjectUpdate();
}
