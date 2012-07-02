/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "UpdateModifiedObjectsThread.h"

void UpdateModifiedObjectsThread::run() {
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

void UpdateModifiedObjectsThread::commitTransaction() {
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
		blockMutex.unlock();

		ObjectDatabaseManager::instance()->commitLocalTransaction();
	}
}

void UpdateModifiedObjectsThread::commitObjectsToDatabase() {
	try {
		Time start;

		if (objectsToUpdate != NULL) {
			int j = 0;

			for (int i = startOffset; i <= endOffset; ++i) {
				DistributedObject* object = objectsToUpdate->get(i);
				
				if (object->_isDeletedFromDatabase()) {
					object->_setDeletedFromDatabase(false);
				}

				if (objectManager->commitUpdatePersistentObjectToDB(object) == 0)
					++j;

				Thread::yield();
			}

			StringBuffer msg;
			msg << "thread " << threadId << " copied " << j <<  " modified objects into ram in " << start.miliDifference() << " ms";
			objectManager->info(msg.toString(), true);
		}

		start.updateToCurrentTime();

		if (objectsToDelete != NULL) {
			for (int i = 0; i < objectsToDelete->size(); ++i) {
				DistributedObject* object = objectsToDelete->get(i);

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
