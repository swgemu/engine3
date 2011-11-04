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

	template<class O> class TransactionalObjectHeader : public Object {
	protected:
		AtomicReference<Transaction*> ownerTransaction;

		AtomicReference<TransactionalObjectHandle<O>* > last;

		Reference<Object*> object;

	public:
		TransactionalObjectHeader(O obj) {
			setObject(obj);

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

		O getForDirty() {
			return object;
		}

		bool isCurrentVersion(Object* obj);

		bool toBinaryStream(ObjectOutputStream* stream) {
			return false;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			return false;
		}

	protected:
		Reference<TransactionalObjectHandle<O>*> createCreationHandle(Transaction* transaction);
		Reference<TransactionalObjectHandle<O>*> createReadOnlyHandle(Transaction* transaction);
		Reference<TransactionalObjectHandle<O>*> createWriteHandle(Transaction* transaction);

		bool acquireObject(Transaction* transaction);

		void releaseObject(TransactionalObjectHandle<O>* handle);

		void discardObject(Transaction* transaction);

		void createObject();

		O getObjectForRead(TransactionalObjectHandle<O>* handle);
		O getObjectForWrite(TransactionalObjectHandle<O>* handle);

		Transaction* getTransaction() const {
			return ownerTransaction;
		}

		bool hasObject(Object* obj) const {
			return object == obj;
		}

		void setObject(O obj) {
			assert(obj != NULL);
			assert(object == NULL);

			object = obj;
			TransactionalObjectHeader<O>::createObject();

			assert(object != NULL);
		}

		friend class Transaction;
		friend class TransactionalObjectHandle<O>;
	};

	template<class O> Reference<TransactionalObjectHandle<O>*> TransactionalObjectHeader<O>::createCreationHandle(Transaction* transaction) {
		Reference<TransactionalObjectHandle<O>*> handle = new TransactionalObjectHandle<O>();
		handle->initialize(this, TransactionalObjectHandle<O>::CREATE, transaction);

		return handle;
	}

	template<class O> Reference<TransactionalObjectHandle<O>*> TransactionalObjectHeader<O>::createReadOnlyHandle(Transaction* transaction) {
		Reference<TransactionalObjectHandle<O>*> handle = new TransactionalObjectHandle<O>();
		handle->initialize(this, TransactionalObjectHandle<O>::READ, transaction);

		return handle;
	}

	template<class O> Reference<TransactionalObjectHandle<O>*> TransactionalObjectHeader<O>::createWriteHandle(Transaction* transaction) {
		Reference<TransactionalObjectHandle<O>*> handle = new TransactionalObjectHandle<O>();
		handle->initialize(this, TransactionalObjectHandle<O>::WRITE, transaction);

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

	template<class O> bool TransactionalObjectHeader<O>::isCurrentVersion(Object* obj) {
		if (TransactionalObjectHeader<O>::ownerTransaction != NULL && TransactionalObjectHeader<O>::ownerTransaction != Transaction::currentTransaction())
			return false;

		return object == obj;
	}

	template<class O> void TransactionalObjectHeader<O>::releaseObject(TransactionalObjectHandle<O>* handle) {
		object = handle->getObjectLocalCopy()->clone(NULL);

		assert(object != NULL);

		//TransactionalObjectHeader<O>::last = NULL;

		//ownerTransaction->release();

		TransactionalObjectHeader<O>::ownerTransaction = NULL;
	}

	template<class O> O TransactionalObjectHeader<O>::getObjectForRead(TransactionalObjectHandle<O>* handle) {
		Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

		assert(object != NULL);

		if (transaction != NULL) {
			if (!transaction->isCommited())
				return dynamic_cast<O>(object.get());
			else
				throw TransactionAbortedException();

			//return ownerTransaction->getOpenedObject(this);
		} else {
			//add(handle);
			return dynamic_cast<O>(object.get);
		}
	}

	template<class O> O TransactionalObjectHeader<O>::getObjectForWrite(TransactionalObjectHandle<O>* handle) {
		Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

		/*if (transaction != NULL) {
			 	 if (!transaction->isCommited())
			 	 	 return object;
			 	 else
			 	 	 throw TransactionAbortedException();

			 	 //return ownerTransaction->getOpenedObject(this);
			 } else {*/

		this->add(handle);

		assert(object != NULL);

		O objectToReturn = dynamic_cast<O>(object.get());

		assert(objectToReturn != NULL);
		return objectToReturn;
		//}
	}

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTIONALOBJECTHEADER_H_ */
