/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "UpdateModifiedObjectsThread.h"

UpdateModifiedObjectsThread::UpdateModifiedObjectsThread(int id, DOBObjectManager* manager, int cpu) {
	objectManager = manager;
	objectsToUpdate = nullptr;
	objectsToDelete = nullptr;
	startOffset = 0;
	endOffset = 0;
	doRun = true;
	waitingToStart = true;
	threadId = id;
	working = false;
	finishedCommiting = false;
	waitingToCommit = false;

	transaction = nullptr;

	this->cpu = cpu;
}

void UpdateModifiedObjectsThread::run() NO_THREAD_SAFETY_ANALYSIS {
	assignToCPU(cpu);

	while (doRun) {
		blockMutex.lock();

		waitingToStart = true;

		while (!copyRAMFinished) {
			waitCondition.wait(&blockMutex);

			waitingToStart = false;

			working = true;
			finishedCommiting = false;

			commitObjectsToDatabase();

			working = false;

			objectsToUpdate = nullptr;
			objectsToDelete = nullptr;

			finishedWorkContition.broadcast(&blockMutex);
		}

		commitTransaction();

		copyRAMFinished = false;
	}
}

void UpdateModifiedObjectsThread::commitTransaction() NO_THREAD_SAFETY_ANALYSIS {
	bool rootBroker = DistributedObjectBroker::instance()->isRootBroker();

	if (transaction != nullptr) {
		waitingToCommit = true;

		waitMasterTransaction.wait(&blockMutex);

		Timer clockTimer(Time::MONOTONIC_TIME);
		clockTimer.start();

		ObjectDatabaseManager::instance()->commitLocalTransaction(transaction);

		uint64 delta = clockTimer.stop();

		StringBuffer msg;
		msg << "thread " << threadId << " commited objects into database in " << delta / 1000000 << " ms";
		objectManager->info(msg.toString(), true);

		transaction = nullptr;

		finishedCommiting = true;

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

		if (objectsToUpdate != nullptr) {
			int j = 0;

			for (int i = startOffset; i < endOffset; ++i) {
				DistributedObject* object = objectsToUpdate->get(i);

				if (objectManager->commitUpdatePersistentObjectToDB(object) == 0)
					++j;

				//Thread::yield();
			}

			StringBuffer msg;
			msg << "thread " << threadId << " copied " << j <<  " modified objects into ram in " << start.miliDifference() << " ms";
			objectManager->info(msg.toString(), true);
		}


		start.updateToCurrentTime();

		if (objectsToDelete != nullptr) {
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
