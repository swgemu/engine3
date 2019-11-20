/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef UPDATEMODIFIEDOBJECTSTHREAD_H_
#define UPDATEMODIFIEDOBJECTSTHREAD_H_

#include "engine/engine.h"

#include "system/thread/atomic/AtomicBoolean.h"
#include "DOBObjectManager.h"

namespace engine {
	namespace ORB {

	class UpdateModifiedObjectsThread : public Thread {
		DOBObjectManager* objectManager;

		ArrayList<DistributedObject*>* objectsToUpdate;
		ArrayList<DistributedObject*>* objectsToDelete;
		int startOffset;
		int endOffset;
		int threadId;

		Mutex blockMutex;
		Condition waitCondition;
		Condition finishedWorkCondition;
		Condition waitMasterTransaction;

		AtomicBoolean copyRAMFinished;
		AtomicBoolean doRun;
		AtomicBoolean working;
		AtomicBoolean finishedCommiting;
		AtomicBoolean waitingToCommit;
		AtomicBoolean waitingToStart;
		AtomicBoolean loadedDBHandles;

		engine::db::berkeley::Transaction* transaction;

		int cpu;

	public:
		UpdateModifiedObjectsThread(int id, DOBObjectManager* manager, int cpu);

		void run() NO_THREAD_SAFETY_ANALYSIS;

		void commitObjectsToDatabase();
		void commitTransaction() NO_THREAD_SAFETY_ANALYSIS;

		inline void setObjectsToUpdateVector(ArrayList<DistributedObject*>* objectsToUpdate) {
			this->objectsToUpdate = objectsToUpdate;
		}

		inline void setObjectsToDeleteVector(ArrayList<DistributedObject*>* objectsToDelete) {
			this->objectsToDelete = objectsToDelete;
		}

		inline void setTransaction(engine::db::berkeley::Transaction* trans) {
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

			signalCopyFinished();

			waitFinishedWork();

			join();
		}

		inline void setRAMCopyFinished(bool val) {
			copyRAMFinished = val;
		}

		inline bool hasFinishedCommiting() const {
			return finishedCommiting;
		}

		inline void signalMasterTransactionFinish() {
			blockMutex.lock();

			waitMasterTransaction.broadcast(&blockMutex);

			blockMutex.unlock();
		}

		inline void signalCopyFinished() {
			blockMutex.lock();

			setRAMCopyFinished(true);

			waitCondition.broadcast(&blockMutex);

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

			while (doRun && waitingToStart && objectsToUpdate != nullptr) {
				waitCondition.broadcast(&blockMutex);
				blockMutex.unlock();
				blockMutex.lock();
			}

			if (working)
				finishedWorkCondition.wait(&blockMutex);

			blockMutex.unlock();
		}
	};

  } // namespace ORB
} // namespace engine

#endif /* UPDATEMODIFIEDOBJECTSTHREAD_H_ */
