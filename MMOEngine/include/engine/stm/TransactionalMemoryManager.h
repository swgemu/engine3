/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_TRANSACTIONALMEMORYMANAGER_H_
#define ENGINE_STM_TRANSACTIONALMEMORYMANAGER_H_

#include "system/lang.h"

#include "system/mm/MemoryManager.h"

#include "engine/util/Singleton.h"

#include "engine/stm/task/TransactionalTaskManager.h"

#include "engine/stm/orb/TransactionalObjectManager.h"

#include "engine/stm/service/TransactionalSocketManager.h"
#include "engine/stm/service/TransactionalBaseClientManager.h"

#include "Transaction.h"

#include "TransactionalReference.h"
#include "TransactionalWeakReference.h"

namespace engine {
  namespace stm {

	class TransactionalMemoryManager : public Singleton<TransactionalMemoryManager>, public MemoryManager, public Logger {
		TransactionalTaskManager* taskManager;

		TransactionalObjectManager* objectManager;

		TransactionalSocketManager* socketManager;

		TransactionalBaseClientManager* baseClientManager;

		ThreadLocal<Reference<Transaction*>* > currentTransaction;

		AtomicInteger transactionID;

		AtomicInteger startedTransactions;
		AtomicInteger commitedTransactions;
		AtomicInteger abortedTransactions;
		AtomicInteger retryConflicts;
		AtomicInteger failedOnAcquireRW;
		AtomicInteger failedToResolveConflict;
		AtomicInteger failedToResolveConflictReleasedTransaction;
		AtomicInteger failedToNotUNDECIDED;
		AtomicInteger failedToExceptions;

		ProtectedHeap objectHeap;
		Mutex heapLock;

		bool reclaiming;

		ThreadLocal<Vector<Object*>* > reclamationList;

		ReadWriteLock blockLock;

	public:
		static void commitPureTransaction();

		static void closeThread() {
			instance()->reclaimObjects(5000, 0);
		}

		Object* create(size_t size);
		void destroy(Object* object);

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

		inline TransactionalBaseClientManager* getBaseClientManager() {
			return baseClientManager;
		}

		inline void blockTransactions() {
			Transaction::blockLock.wlock();
		}

		inline void increaseFailedByExceptions() {
			failedToExceptions.increment();
		}

		inline void increaseFailedByObjectChanged() {
			failedOnAcquireRW.increment();
		}

		inline void unblockTransactions() {
			Transaction::blockLock.unlock();
		}

		void setKernelMode();
		void setUserMode();

	protected:
		TransactionalMemoryManager();

		~TransactionalMemoryManager();

		Transaction* getTransaction();

		void startTransaction(Transaction* transaction);

		void commitTransaction();

		void abortTransaction();

		void retryTransaction();

		void reclaim(Object* object);

		void setCurrentTransaction(Transaction* transaction);

		void reclaimObjects(int objectsToSpare = 0, int maxObjectsToReclaim = 0);

		Vector<Object*>* getReclamationList();

		friend class SingletonWrapper<TransactionalMemoryManager>;
		friend class Transaction;
};

  } // namespace stm
} // namespace engine

#include "TransactionalObjectHeader.h"

#include "engine/stm/mm/TransactionalObjectCloner.h"

#endif /* ENGINE_STM_TRANSACTIONALMEMORYMANAGER_H_ */
