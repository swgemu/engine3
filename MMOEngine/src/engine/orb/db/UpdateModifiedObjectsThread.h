/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef UPDATEMODIFIEDOBJECTSTHREAD_H_
#define UPDATEMODIFIEDOBJECTSTHREAD_H_

#include "engine/engine.h"

#include "DOBObjectManager.h"

namespace engine {
	namespace ORB {

	class UpdateModifiedObjectsThread : public Thread {
		DOBObjectManager* objectManager;

		Vector<DistributedObject*>* objectsToUpdate;
		Vector<DistributedObject*>* objectsToDelete;
		int startOffset;
		int endOffset;
		int threadId;

		Mutex blockMutex;
		Condition waitCondition;
		Condition finishedWorkContition;
		Condition waitMasterTransaction;

		bool doRun;
		bool working;
		bool finishedCommiting;
		bool waitingToCommit;
		bool waitingToStart;

		engine::db::berkley::Transaction* transaction;

	public:
		UpdateModifiedObjectsThread(int id, DOBObjectManager* manager) {
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
		}

		void run();

		void commitObjectsToDatabase();
		void commitTransaction();

		inline void setObjectsToUpdateVector(Vector<DistributedObject*>* objectsToUpdate) {
			this->objectsToUpdate = objectsToUpdate;
		}

		inline void setObjectsToDeleteVector(Vector<DistributedObject*>* objectsToDelete) {
			this->objectsToDelete = objectsToDelete;
		}

		inline void setTransaction(engine::db::berkley::Transaction* trans) {
			transaction = trans;
		}

		inline void setStartOffset(int offset) {
			startOffset = offset;
		}

		inline void setEndOffset(int offset) {
			endOffset = offset;
		}

		inline void stopWork() {
			doRun = false;

			signalActivity();

			join();
		}

		inline bool hasFinishedCommiting() {
			//blockMutex.lock();

			bool res = finishedCommiting;

			//blockMutex.unlock();

			return res;
		}

		inline void signalMasterTransactionFinish() {
			blockMutex.lock();

			waitMasterTransaction.broadcast(&blockMutex);

			blockMutex.unlock();
		}

		inline void signalActivity() {
			blockMutex.lock();

			working = true;

			waitCondition.broadcast(&blockMutex);

			blockMutex.unlock();
		}

		inline void waitFinishedWork() {
			blockMutex.lock();

			while (waitingToStart && objectsToUpdate != NULL) {
				waitCondition.broadcast(&blockMutex);
				blockMutex.unlock();
				blockMutex.lock();
			}

			if (working)
				finishedWorkContition.wait(&blockMutex);

			blockMutex.unlock();
		}
	};

  } // namespace ORB
} // namespace engine

#endif /* UPDATEMODIFIEDOBJECTSTHREAD_H_ */
