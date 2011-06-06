/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_TRANSACTIONALMEMORYMANAGER_H_
#define ENGINE_STM_TRANSACTIONALMEMORYMANAGER_H_

#include "system/lang.h"

#include "engine/util/Singleton.h"

#include "engine/stm/task/TransactionalTaskManager.h"

#include "engine/stm/orb/TransactionalObjectManager.h"

#include "engine/stm/service/TransactionalSocketManager.h"

#include "Transaction.h"

#include "TransactionalObjectHeader.h"

#include "TransactionalReference.h"

namespace engine {
  namespace stm {

	class TransactionalMemoryManager : public Singleton<TransactionalMemoryManager>, public MemoryManager, public Logger {
		TransactionalTaskManager* taskManager;

		TransactionalObjectManager* objectManager;

		TransactionalSocketManager* socketManager;

		ThreadLocal<Transaction*> currentTransaction;

		AtomicInteger transactionID;

		AtomicInteger startedTransactions;
		AtomicInteger commitedTransactions;
		AtomicInteger abortedTransactions;
		AtomicInteger retryConflicts;

		ThreadLocal<Vector<Object*>* > reclamationList;

	public:
		static void commitPureTransaction();

		static void closeThread() {
			instance()->reclaimObjects();
		}

		void printStatistics();

		TransactionalTaskManager* getTaskManager() {
			return taskManager;
		}

		TransactionalObjectManager* getObjectManager() {
			return objectManager;
		}

		TransactionalSocketManager* getSocketManager() {
			return socketManager;
		}

	protected:
		TransactionalMemoryManager();

		~TransactionalMemoryManager();

		Transaction* getTransaction();

		void startTransaction(Transaction* transaction);

		void commitTransaction();

		void abortTransaction();

		void retryTransaction();

		void reclaim(Object* object);

		void reclaimObjects(int objectsToSpare = 0, int maxObjectsToReclaim = 0);

		Vector<Object*>* getReclamationList();

		friend class SingletonWrapper<TransactionalMemoryManager>;
		friend class Transaction;
};

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTIONALMEMORYMANAGER_H_ */
