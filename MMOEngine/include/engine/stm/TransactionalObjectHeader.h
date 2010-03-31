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

	template<class O> class TransactionalObjectHandle;

	template<class O> class TransactionalObjectHeader {
		O object;

		AtomicReference<Transaction> ownerTransaction;

	public:
		TransactionalObjectHeader() {
			object = NULL;

			ownerTransaction = NULL;
		}

		TransactionalObjectHeader(O obj) {
			object = obj;

			ownerTransaction = NULL;
		}

		O get();

		O getForUpdate();

	protected:
		TransactionalObjectHandle<O>* createHandle();

		bool acquireObject(Transaction* transaction);

		void releaseObject(TransactionalObjectHandle<O>* handle);

		bool discardObject(Transaction* transaction);

		O getObject() const {
			return object;
		}

		Transaction* getTransaction() const {
			return ownerTransaction;
		}

		bool isNull() {
			return object != NULL;
		}

		void setObject(O obj) {
			object = obj;
		}

		friend class Transaction;
		friend class TransactionalObjectHandle<O>;
	};

	template<class O> TransactionalObjectHandle<O>* TransactionalObjectHeader<O>::createHandle() {
		return new TransactionalObjectHandle<O>(this);
	}

	template<class O> O TransactionalObjectHeader<O>::get() {
		return Transaction::currentTransaction()->openObject<O>(this);
	}

	template<class O> O TransactionalObjectHeader<O>::getForUpdate() {
		return Transaction::currentTransaction()->openObjectForWrite<O>(this);
	}

	template<class O> bool TransactionalObjectHeader<O>::acquireObject(Transaction* transaction) {
		/*System::out.println("[" + Thread::getCurrentThread()->getName() +"] acquiring (" + String::valueOf((uint64) ownerTransaction.get()) +
				", " + String::valueOf((uint64) transaction) + ")" );*/

		return ownerTransaction.compareAndSet(NULL, transaction);
	}

	template<class O> void TransactionalObjectHeader<O>::releaseObject(TransactionalObjectHandle<O>* handle) {
		O oldObject = handle->getObject();

		object = handle->getObjectLocalCopy();

		//delete oldObject;
		ownerTransaction = NULL;
	}

	template<class O> bool TransactionalObjectHeader<O>::discardObject(Transaction* transaction) {
		return ownerTransaction.compareAndSet(transaction, NULL);
	}

  } // namespace stm
} // namespace engine


#endif /* ENGINE_STM_TRANSACTIONALOBJECTHEADER_H_ */
