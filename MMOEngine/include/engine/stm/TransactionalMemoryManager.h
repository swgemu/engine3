/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_TRANSACTIONALMEMORYMANAGER_H_
#define ENGINE_STM_TRANSACTIONALMEMORYMANAGER_H_

#include "system/lang.h"

#include "engine/util/Singleton.h"

#include "engine/stm/service/TransactionalSocketManager.h"

#include "Transaction.h"

#include "TransactionalObjectHeader.h"

namespace engine {
  namespace stm {

	class TransactionalMemoryManager : public Singleton<TransactionalMemoryManager> {
		TransactionalSocketManager* socketManager;

		ThreadLocal<Transaction> currentTransaction;

	public:
		TransactionalSocketManager* getSocketManager() {
			return socketManager;
		}

	protected:
		TransactionalMemoryManager();

		Transaction* getTransaction();

		void clearTransaction();

		friend class SingletonWrapper<TransactionalMemoryManager>;
		friend class Transaction;
};

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTIONALMEMORYMANAGER_H_ */
