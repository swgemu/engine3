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
	protected:
		AtomicReference<Transaction*> ownerTransaction;

		AtomicReference<TransactionalObjectHandle<O>* > last;

	public:
		TransactionalObjectHeader() {
			ownerTransaction = NULL;
			last = NULL;
		}

		virtual ~TransactionalObjectHeader() {

		}

		O get();

		O getForUpdate();

		Reference<TransactionalObjectHandle<O>*> add(TransactionalObjectHandle<O>* handle);

		inline Reference<TransactionalObjectHandle<O>*> getLastHandle() {
			return add(NULL);
		}

		virtual O getForDirty() = 0;

		virtual bool isCurrentVersion(Object* obj) = 0;

		bool toBinaryStream(ObjectOutputStream* stream) {
			return false;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			return false;
		}

	protected:
		Reference<TransactionalObjectHandle<O>*> createReadOnlyHandle(Transaction* transaction);
		Reference<TransactionalObjectHandle<O>*> createWriteHandle(Transaction* transaction);

		bool acquireObject(Transaction* transaction);

		virtual void releaseObject(TransactionalObjectHandle<O>* handle) = 0;

		void discardObject(Transaction* transaction);

		void createObject();

		virtual Object* getObjectForRead(TransactionalObjectHandle<O>* handle) = 0;
		virtual Object* getObjectForWrite(TransactionalObjectHandle<O>* handle) = 0;

		Transaction* getTransaction() const {
			return ownerTransaction;
		}

		virtual bool hasObject(Object* obj) const = 0;

		virtual bool isNull() = 0;

		virtual void setObject(O obj) = 0;

		friend class Transaction;
		friend class TransactionalObjectHandle<O>;
	};

	template<class O> Reference<TransactionalObjectHandle<O>*> TransactionalObjectHeader<O>::createReadOnlyHandle(Transaction* transaction) {
		Reference<TransactionalObjectHandle<O>*> handle = new TransactionalObjectHandle<O>();
		handle->initialize(this, false, transaction);

		return handle;
	}

	template<class O> Reference<TransactionalObjectHandle<O>*> TransactionalObjectHeader<O>::createWriteHandle(Transaction* transaction) {
		Reference<TransactionalObjectHandle<O>*> handle = new TransactionalObjectHandle<O>();
		handle->initialize(this, true, transaction);

		return handle;
	}

	template<class O> void TransactionalObjectHeader<O>::createObject() {
		Transaction::currentTransaction()->createObject<O>(this);
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

	template<class O> void TransactionalObjectHeader<O>::discardObject(Transaction* transaction) {
		ownerTransaction.compareAndSet(transaction, NULL);
	}

	template <class O> Reference<TransactionalObjectHandle<O>*> TransactionalObjectHeader<O>::add(TransactionalObjectHandle<O>* handle) {
		if (handle != NULL)
			handle->acquire();

		while (true) {
			TransactionalObjectHandle<O>* lastRef = last.get();

			if (last.compareAndSet(lastRef, handle)) {
				if (lastRef != NULL && handle != NULL) {
					lastRef->setPrevious(handle);
				}

				Reference<TransactionalObjectHandle<O>*> strong = lastRef;

				if (lastRef != NULL)
					lastRef->release();

				return strong;
			}

			Thread::yield();
		}
	}

  } // namespace stm
} // namespace engine

#include "mm/TransactionalWeakObjectHeader.h"

#include "mm/TransactionalStrongObjectHeader.h"

#endif /* ENGINE_STM_TRANSACTIONALOBJECTHEADER_H_ */
