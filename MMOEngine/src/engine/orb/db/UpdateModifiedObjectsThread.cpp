/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "UpdateModifiedObjectsThread.h"

UpdateModifiedObjectsThread::UpdateModifiedObjectsThread(int id, DOBObjectManager* manager, int cpu) {
	objectManager = manager;
	objectsToUpdate = NULL;
	objectsToDelete = NULL;
	startOffset = 0;
	endOffset = 0;
	doRun = true;
	waitingToStart = true;
	threadId = id;
	working = false;
	finishedCommiting = false;
	waitingToCommit = false;

	transaction = NULL;

	this->cpu = cpu;
}

void UpdateModifiedObjectsThread::run() NO_THREAD_SAFETY_ANALYSIS {
	assignToCPU(cpu);

	while (doRun) {
		blockMutex.lock();

		waitingToStart = true;

		waitCondition.wait(&blockMutex);

		waitingToStart = false;

		working = true;
		finishedCommiting = false;

		commitObjectsToDatabase();

		working = false;

		objectsToUpdate = NULL;
		objectsToDelete = NULL;

		finishedWorkContition.broadcast(&blockMutex);

		commitTransaction();
	}
}

void UpdateModifiedObjectsThread::commitTransaction() NO_THREAD_SAFETY_ANALYSIS {
	bool rootBroker = DistributedObjectBroker::instance()->isRootBroker();

	if (transaction != NULL) {
		waitingToCommit = true;

		waitMasterTransaction.wait(&blockMutex);

		Time start;

		ObjectDatabaseManager::instance()->commitLocalTransaction(transaction);

		StringBuffer msg;
		msg << "thread " << threadId << " commited objects into database in " << start.miliDifference() << " ms";
		objectManager->info(msg.toString(), true);

		finishedCommiting = true;

		transaction = NULL;

		blockMutex.unlock();
	} else {
		finishedCommiting = true;

		blockMutex.unlock();

		if (rootBroker) {
			ObjectDatabaseManager::instance()->commitLocalTransaction();
		}
	}
}

void UpdateModifiedObjectsThread::commitObjectsToDatabase() {
	try {
		Time start;

		if (objectsToUpdate != NULL) {
			int j = 0;

			for (int i = startOffset; i <= endOffset; ++i) {
				DistributedObject* object = objectsToUpdate->getUnsafe(i);

				if (objectManager->commitUpdatePersistentObjectToDB(object) == 0)
					++j;

				//Thread::yield();
			}

			StringBuffer msg;
			msg << "thread " << threadId << " copied " << j <<  " modified objects into ram in " << start.miliDifference() << " ms";
			objectManager->info(msg.toString(), true);
		}


		start.updateToCurrentTime();

		if (objectsToDelete != NULL) {
			for (int i = 0; i < objectsToDelete->size(); ++i) {
				DistributedObject* object = objectsToDelete->getUnsafe(i);

				if (!object->_isDeletedFromDatabase()) {
					objectManager->commitDestroyObjectToDB(object->_getObjectID());
					object->_setDeletedFromDatabase(true);
				}
			}

			StringBuffer msg;
			msg << "thread " << threadId << " commited " << objectsToDelete->size() <<  " objects for deletion into ram in " << start.miliDifference() << " ms";
			objectManager->info(msg.toString(), true);
		}
	} catch (Exception& e) {
		objectManager->error(e.getMessage());
	} catch (...) {
		objectManager->error("unreported exception caught");

		throw;
	}
}
