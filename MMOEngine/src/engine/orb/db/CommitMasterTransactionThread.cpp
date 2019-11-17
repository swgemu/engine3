/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
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

CommitMasterTransactionThread::CommitMasterTransactionThread() : Logger("CommitMasterTransactionThread") {
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

void CommitMasterTransactionThread::startWatch(engine::db::berkeley::Transaction* trans, Vector<UpdateModifiedObjectsThread*>* workers, int number, Vector<DistributedObject* >* objectsToCollect) {
	fatal(workers != nullptr) << "workers is null";
	fatal(objectsToCollect != nullptr) << "objectsToCollect is null";

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

	if (objectsToDeleteFromRam == nullptr) {
		return i;
	}

	static const int objectsToDeletePerSleep = Core::getIntProperty("ObjectManager.objectsToDeletePerSleepThrottle", 10000);
	static const int actualObjectsToDeleteSleep = Core::getIntProperty("ObjectManager.actualObjectsToDeleteSleepThrottle", 100);
	static const int objectsToDeleteSleep = Core::getIntProperty("ObjectManager.objectsToDeleteSleepThrottle", 25);

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

		if ((((j + 1) % objectsToDeletePerSleep) == 0) || ((i + 1) % actualObjectsToDeleteSleep ) == 0) {
			locker.release();

			Thread::sleep(objectsToDeleteSleep);
		}
	}

	delete objectsToDeleteFromRam;
	objectsToDeleteFromRam = nullptr;

	return i;
}

void CommitMasterTransactionThread::commitData() NO_THREAD_SAFETY_ANALYSIS {
	for (int i = 0; i < numberOfThreads; ++i) {
		UpdateModifiedObjectsThread* worker = threads->get(i);

		while (!worker->hasFinishedCommiting()) {
			worker->signalMasterTransactionFinish();

			static const int databaseWatchThreadThrottle = Core::getIntProperty("ObjectManager.databaseWatchThreadThrottle", 500);

			Thread::sleep(databaseWatchThreadThrottle);
		}
	}

	DOBObjectManager* objectManager = DistributedObjectBroker::instance()->getObjectManager();

	if (DistributedObjectBroker::instance()->isRootBroker()) {
		ObjectDatabaseManager::instance()->commitTransaction(transaction);
		ObjectDatabaseManager::instance()->checkpoint();

		objectManager->onCommitData();
	}

	objectManager->info(true) << "master transaction commited";

	objectManager->checkCommitedObjects();

	objectManager->info(true) << "starting garbage collection for " << objectsToDeleteFromRam->size() << " candidates";

	int objs = garbageCollect(objectManager);

	objectManager->info(true) << "deleted from ram " << objs << " objects";

	objectManager->finishObjectUpdate();
}
