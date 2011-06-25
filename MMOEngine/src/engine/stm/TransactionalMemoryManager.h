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
#include "engine/stm/service/TransactionalBaseClientManager.h"

#include "Transaction.h"

#include "TransactionalObjectHeader.h"

#include "TransactionalReference.h"
#include "TransactionalWeakReference.h"

namespace engine {
  namespace stm {

  class DeletedTrace {
	  StackTrace stackTrace;
	  Thread* thread;
	  int tid;
  public:
	  DeletedTrace() {
		  thread = Thread::getCurrentThread();
		  tid = 0;
	  }

	  void print() {
		  System::out << "TID:" << tid << " Thread: " << hex << thread << endl;
		  stackTrace.print();
	  }

	  void setTid(int tid) {
		  this->tid = tid;
	  }


  };

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


		bool reclaiming;

		ThreadLocal<Vector<Object*>* > reclamationList;

		VectorMap<uint64, DeletedTrace*> deleted;
		Mutex deletedMutex;
		ReadWriteLock blockLock;

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

		inline TransactionalBaseClientManager* getBaseClientManager() {
			return baseClientManager;
		}

		inline void blockTransactions() {
			Transaction::blockLock.wlock();
		}

		inline void increaseFailedByExceptions() {
			failedToExceptions.increment();
		}

		inline void unblockTransactions() {
			Transaction::blockLock.unlock();
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

		void create(Object* object);

		void setCurrentTransaction(Transaction* transaction);

		void reclaimObjects(int objectsToSpare = 0, int maxObjectsToReclaim = 0);

		Vector<Object*>* getReclamationList();

		friend class SingletonWrapper<TransactionalMemoryManager>;
		friend class Transaction;
};

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTIONALMEMORYMANAGER_H_ */
