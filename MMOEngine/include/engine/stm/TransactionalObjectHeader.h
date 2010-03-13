/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_TRANSACTIONALOBJECTHEADER_H_
#define ENGINE_STM_TRANSACTIONALOBJECTHEADER_H_

#include "system/lang.h"

#include "TransactionalObject.h"

#include "Transaction.h"

namespace engine {
  namespace stm {

	class TransactionalObjectHeader {
		TransactionalObject* object;

		Transaction* ownerTransaction;

	public:
		TransactionalObjectHeader(TransactionalObject* obj) {
			object = obj;

			ownerTransaction = NULL;
		}

		TransactionalObject* get();

		TransactionalObject* getForUpdate();

	protected:
		bool acquire(Transaction* transaction);

		void release(TransactionalObject* newobject);

		TransactionalObject* getObject() {
			return object;
		}

		Transaction* getTransaction() {
			return ownerTransaction;
		}

		friend class Transaction;
		friend class TransactionalObjectHandle;
	};

  } // namespace stm
} // namespace engine


#endif /* ENGINE_STM_TRANSACTIONALOBJECTHEADER_H_ */
