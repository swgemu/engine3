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

	doRun = true;
}

void CommitMasterTransactionThread::startWatch(engine::db::berkley::Transaction* trans, Vector<UpdateModifiedObjectsThread*>* workers, int number) {
	assert(trans != NULL);
	assert(workers != NULL);

	transaction = trans;
	threads = workers;
	numberOfThreads = number;

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

		blockMutex.unlock();
	}
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
	objectManager->finishObjectUpdate();
}
