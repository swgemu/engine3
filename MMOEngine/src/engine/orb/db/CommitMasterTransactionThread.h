/*
 * CommitMasterTransactionThread.h
 *
 *  Created on: 18/02/2012
 *      Author: victor
 */

#ifndef COMMITMASTERTRANSACTIONTHREAD_H_
#define COMMITMASTERTRANSACTIONTHREAD_H_

#include "engine/engine.h"

#include "UpdateModifiedObjectsThread.h"

namespace engine {
	namespace ORB {

	class CommitMasterTransactionThread : public Thread, public Singleton<CommitMasterTransactionThread>, public Object {
		Mutex blockMutex;
		Condition waitCondition;

		engine::db::berkley::Transaction* transaction;
		Vector<UpdateModifiedObjectsThread*>* threads;
		int numberOfThreads;

		bool doRun;

	public:
		CommitMasterTransactionThread();

		void run();

		void startWatch(engine::db::berkley::Transaction* trans, Vector<UpdateModifiedObjectsThread*>* workers, int number);

		void commitData();
	};

  } // namespace ORB
} // namespace engine

#endif /* COMMITMASTERTRANSACTIONTHREAD_H_ */
