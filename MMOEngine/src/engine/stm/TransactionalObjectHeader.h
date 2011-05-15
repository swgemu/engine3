/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_TRANSACTIONALOBJECTHEADER_H_
#define ENGINE_STM_TRANSACTIONALOBJECTHEADER_H_

#include "system/lang.h"

#include "TransactionalMemoryManager.h"

#include "TransactionAbortedException.h"

#include "Transaction.h"

namespace engine {
  namespace stm {

	template<class O> class TransactionalObjectHandle;

	template<class O> class TransactionalObjectHeader {
		Reference<Object*> object;

		AtomicReference<Transaction> ownerTransaction;

	public:
		TransactionalObjectHeader() {
			object = NULL;

			ownerTransaction = NULL;
		}

		TransactionalObjectHeader(O obj) {
			setObject(obj);

			ownerTransaction = NULL;
		}

		O get();

		O getForUpdate();

	protected:
		TransactionalObjectHandle<O>* createReadOnlyHandle();

		TransactionalObjectHandle<O>* createWriteHandle();

		bool acquireObject(Transaction* transaction);

		void releaseObject(TransactionalObjectHandle<O>* handle);

		void discardObject(Transaction* transaction);

		Object* getObject();

		Transaction* getTransaction() const {
			return ownerTransaction;
		}

		bool isCurrentVersion(Object* obj);

		bool hasObject(Object* obj) const {
			return object == obj;
		}

		bool isNull() {
			return object == NULL;
		}

		void setObject(O obj) {
			object = dynamic_cast<Object*>(obj);
		}

		friend class Transaction;
		friend class TransactionalObjectHandle<O>;
	};

	template<class O> TransactionalObjectHandle<O>* TransactionalObjectHeader<O>::createReadOnlyHandle() {
		TransactionalObjectHandle<O>* handle = new TransactionalObjectHandle<O>(this, false);

		return handle;
	}

	template<class O> TransactionalObjectHandle<O>* TransactionalObjectHeader<O>::createWriteHandle() {
		TransactionalObjectHandle<O>* handle = new TransactionalObjectHandle<O>(this, true);

		return handle;
	}

	template<class O> Object* TransactionalObjectHeader<O>::getObject() {
		if (ownerTransaction != NULL)
			throw TransactionAbortedException();
			//return ownerTransaction->getOpenedObject(this);
		 else
			return object;
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
		object = handle->getObjectLocalCopy();

		//ownerTransaction->release();

		ownerTransaction = NULL;
	}

	template<class O> void TransactionalObjectHeader<O>::discardObject(Transaction* transaction) {
		ownerTransaction.compareAndSet(transaction, NULL);
	}

	template<class O> bool TransactionalObjectHeader<O>::isCurrentVersion(Object* obj) {
		if (ownerTransaction != NULL && ownerTransaction != Transaction::currentTransaction())
			return false;

		return object == obj;
	}

  } // namespace stm
} // namespace engine


#endif /* ENGINE_STM_TRANSACTIONALOBJECTHEADER_H_ */
