/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_TRANSACTIONALMEMORYMANAGER_H_
#define ENGINE_STM_TRANSACTIONALMEMORYMANAGER_H_

#include "system/lang.h"

#include "engine/util/Singleton.h"

#include "orb/TransactionalObjectManager.h"

#include "engine/stm/service/TransactionalSocketManager.h"

#include "Transaction.h"

#include "TransactionalObjectHeader.h"

#include "TransactionalReference.h"

namespace engine {
  namespace stm {

	class TransactionalMemoryManager : public Singleton<TransactionalMemoryManager>, public MemoryManager, public Logger {
		TransactionalObjectManager* objectManager;

		TransactionalSocketManager* socketManager;

		ThreadLocal<Transaction*> currentTransaction;

		AtomicInteger transactionID;

		AtomicInteger startedTransactions;
		AtomicInteger commitedTransactions;
		AtomicInteger abortedTransactions;

		ThreadLocal<Vector<Object*>* > reclamationList;
		ThreadLocal<bool> isReclaiming;

	public:
		static void commitPureTransaction();

		static void closeThread() {
			instance()->reclaimAll();
		}

		void printStatistics();

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

		void reclaim(Object* object);

		void reclaimAll();

		Vector<Object*>* getReclamationList();

		friend class SingletonWrapper<TransactionalMemoryManager>;
		friend class Transaction;
};

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTIONALMEMORYMANAGER_H_ */
