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
			return get()->toBinaryStream(stream);
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			return getForUpdate()->parseFromBinaryStream(stream);
		}

	protected:
		Reference<TransactionalObjectHandle<O>*> createReadOnlyHandle(Transaction* transaction);
		Reference<TransactionalObjectHandle<O>*> createWriteHandle(Transaction* transaction);

		bool acquireObject(Transaction* transaction);

		virtual void releaseObject(TransactionalObjectHandle<O>* handle) = 0;

		void discardObject(Transaction* transaction);

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

	template<class O> class TransactionalWeakObjectHeader : public TransactionalObjectHeader<O> {
			WeakReference<Object*> object;

	public:
			TransactionalWeakObjectHeader() : TransactionalObjectHeader<O>() {
				object = NULL;
			}

			TransactionalWeakObjectHeader(O obj) : TransactionalObjectHeader<O>() {
				setObject(obj);
			}

			bool isCurrentVersion(Object* obj);

			O getForDirty() {
				return dynamic_cast<O>(object.get());
			}

	protected:
			void setObject(O obj) {
				object = dynamic_cast<Object*>(obj);

				assert(object != NULL);
			}

			bool isNull() {
				return object == NULL;
			}

			bool hasObject(Object* obj) const {
				return object == obj;
			}

			void releaseObject(TransactionalObjectHandle<O>* handle);

			Object* getObjectForRead(TransactionalObjectHandle<O>* handle);
			Object* getObjectForWrite(TransactionalObjectHandle<O>* handle);

	};

	template<class O> class TransactionalStrongObjectHeader : public TransactionalObjectHeader<O> {
		Reference<Object*> object;

	public:
		TransactionalStrongObjectHeader() : TransactionalObjectHeader<O>() {
			object = NULL;
		}

		TransactionalStrongObjectHeader(O obj) : TransactionalObjectHeader<O>() {
			setObject(obj);
		}

		bool isCurrentVersion(Object* obj);

		O getForDirty() {
			return dynamic_cast<O>(object.get());
		}

	protected:
		void setObject(O obj) {
			object = dynamic_cast<Object*>(obj);

			assert(object != NULL);
		}

		bool isNull() {
			return object == NULL;
		}

		bool hasObject(Object* obj) const {
			return object == obj;
		}

		void releaseObject(TransactionalObjectHandle<O>* handle);

		Object* getObjectForRead(TransactionalObjectHandle<O>* handle);
		Object* getObjectForWrite(TransactionalObjectHandle<O>* handle);

	};

	template<class O> Reference<TransactionalObjectHandle<O>*> TransactionalObjectHeader<O>::createReadOnlyHandle(Transaction* transaction) {
		Reference<TransactionalObjectHandle<O>*> handle = new TransactionalObjectHandle<O>();
		handle->initialize(this, false, transaction);

		return handle;
	}

	template<class O> Reference<TransactionalObjectHandle<O>*> TransactionalObjectHeader<O>::createWriteHandle(Transaction* transaction) {
		//assert(object != NULL);
		Reference<TransactionalObjectHandle<O>*> handle = new TransactionalObjectHandle<O>();
		handle->initialize(this, true, transaction);

		return handle;
	}

	template<class O> Object* TransactionalStrongObjectHeader<O>::getObjectForRead(TransactionalObjectHandle<O>* handle) {
		Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

		if (transaction != NULL) {
			if (!transaction->isCommited())
				return object;
			else
			throw TransactionAbortedException();
			//return ownerTransaction->getOpenedObject(this);
		} else {
			//add(handle);
			return object;
		}
	}

	template<class O> Object* TransactionalWeakObjectHeader<O>::getObjectForRead(TransactionalObjectHandle<O>* handle) {
			Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

			if (transaction != NULL) {
				if (!transaction->isCommited())
					return object;
				else
				throw TransactionAbortedException();
				//return ownerTransaction->getOpenedObject(this);
			} else {
				//add(handle);
				return object;
			}
		}

	template<class O> Object* TransactionalStrongObjectHeader<O>::getObjectForWrite(TransactionalObjectHandle<O>* handle) {
		add(handle);

		assert(object != NULL);

		Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

		if (transaction != NULL) {
			//if (transaction->isCommited())
				throw TransactionAbortedException(false);
			//return ownerTransaction->getOpenedObject(this);
		}

		return object;
	}


	template<class O> Object* TransactionalWeakObjectHeader<O>::getObjectForWrite(TransactionalObjectHandle<O>* handle) {
		add(handle);

		assert(object != NULL);

		Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

		if (transaction != NULL) {
			//if (transaction->isCommited())
			throw TransactionAbortedException(false);
			//return ownerTransaction->getOpenedObject(this);
		}

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

	template<class O> void TransactionalStrongObjectHeader<O>::releaseObject(TransactionalObjectHandle<O>* handle) {
		object = handle->getObjectLocalCopy();

		assert(object != NULL);

		//TransactionalObjectHeader<O>::last = NULL;

		//ownerTransaction->release();

		TransactionalObjectHeader<O>::ownerTransaction = NULL;
	}

	template<class O> void TransactionalWeakObjectHeader<O>::releaseObject(TransactionalObjectHandle<O>* handle) {
		object = handle->getObjectLocalCopy();

		assert(object != NULL);

		//ownerTransaction->release();

		//TransactionalObjectHeader<O>::last = NULL;

		TransactionalObjectHeader<O>::ownerTransaction = NULL;
	}

	template<class O> void TransactionalObjectHeader<O>::discardObject(Transaction* transaction) {
		ownerTransaction.compareAndSet(transaction, NULL);
	}

	template<class O> bool TransactionalStrongObjectHeader<O>::isCurrentVersion(Object* obj) {
		if (TransactionalObjectHeader<O>::ownerTransaction != NULL && TransactionalObjectHeader<O>::ownerTransaction != Transaction::currentTransaction())
			return false;

		return object == obj;
	}

	template<class O> bool TransactionalWeakObjectHeader<O>::isCurrentVersion(Object* obj) {
		if (TransactionalObjectHeader<O>::ownerTransaction != NULL && TransactionalObjectHeader<O>::ownerTransaction != Transaction::currentTransaction())
			return false;

		return object == obj;
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


#endif /* ENGINE_STM_TRANSACTIONALOBJECTHEADER_H_ */
