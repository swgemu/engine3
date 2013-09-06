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
	transaction = NULL;
	threads = NULL;

	objectsToDeleteFromRam = NULL;

	doRun = true;
}

void CommitMasterTransactionThread::startWatch(engine::db::berkley::Transaction* trans, Vector<UpdateModifiedObjectsThread*>* workers, int number, Vector<DistributedObject* >* objectsToCollect) {
	assert(trans != NULL);
	assert(workers != NULL);
	assert(objectsToCollect != NULL);

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

		commitData();

		transaction = NULL;
		threads = NULL;
		objectsToDeleteFromRam = NULL;

		blockMutex.unlock();
	}
}

int CommitMasterTransactionThread::garbageCollect(DOBObjectManager* objectManager) {
int i = 0;

//      while (objectsToDeleteFromRam->size() != 0) {
        for (int j = 0; j < objectsToDeleteFromRam->size(); ++j) {
        	DistributedObject* object = objectsToDeleteFromRam->get(j);
                       
                Locker locker(objectManager);
                                        
                //printf("object ref count:%d and updated flag:%d\n", object->getReferenceCount(), object->_isUpdated());
                                                        
                if (object->getReferenceCount() == 2 && (!object->_isUpdated() || object->_isDeletedFromDatabase() || !object->isPersistent())) {
                	objectManager->localObjectDirectory.removeHelper(object->_getObjectID());
                                                                                                                        //localObjectDirectory.removeHelper(object->_getObjectID());
                                                                                                                 
                         ++i;
                         
                         object = NULL;
		} else if (object->_isUpdated() && !object->_isDeletedFromDatabase()) {
                        printf("%s refs:%d\n", TypeInfo<DistributedObject>::getClassName(object).toCharArray(), object->getReferenceCount());		
		}
		
		if ((((j + 1) % 10000) == 0) || ((i + 1) % 100) == 0) {
			locker.release();

			Thread::sleep(25);
		}
	}

	delete objectsToDeleteFromRam;
	objectsToDeleteFromRam = NULL;
        
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

	ObjectDatabaseManager::instance()->commitTransaction(transaction);
	ObjectDatabaseManager::instance()->checkpoint();

	objectManager->onCommitData();

	objectManager->info("master transaction commited", true);

	int candidates = objectsToDeleteFromRam->size();
	objectManager->info("starting garbage collection for " + String::valueOf(candidates) + " candidates", true);

	int objs = garbageCollect(objectManager);

	objectManager->info("deleted from ram " + String::valueOf(objs) + " objects", true);

	objectManager->finishObjectUpdate();
}
