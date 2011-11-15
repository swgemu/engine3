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

//#define STMLINKELISTDHELP
//#define EAGERABORTS

namespace engine {
  namespace stm {

	template<class O> class TransactionalObjectHandle;

	template<class O> class TransactionalObjectHeader : public Object {
	protected:
		Reference<Transaction*> ownerTransaction;

		Reference<Object*> object;

	public:
		TransactionalObjectHeader(O obj) : ownerTransaction(NULL), object(obj) {
			createObject();

			assert(object != NULL);
		}

		virtual ~TransactionalObjectHeader() {
		}

		int compareTo(TransactionalObjectHeader* header) {
			if (this == header)
				return 0;
			else if (this < header)
				return -1;
			else
				return 1;
		}

		O get();

		O getForUpdate();

		Reference<TransactionalObjectHandle<O>*> add(TransactionalObjectHandle<O>* handle);

		inline Reference<TransactionalObjectHandle<O>*> getLastHandle() {
			return add(NULL);
		}

		O getForDirty();

		bool isCurrentVersion(Object* obj);

		bool toBinaryStream(ObjectOutputStream* stream) {
			return false;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			return false;
		}

	protected:
		TransactionalObjectHandle<O>* createCreationHandle(Transaction* transaction);
		TransactionalObjectHandle<O>* createReadOnlyHandle(Transaction* transaction);
		TransactionalObjectHandle<O>* createWriteHandle(Transaction* transaction);

		Transaction* acquireObject(Transaction* transaction);

		void releaseObject(TransactionalObjectHandle<O>* handle, Object* objectCopy);

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

		friend class Transaction;
		friend class TransactionalObjectHandle<O>;
	};

	/*template<class O>
	AtomicLong TransactionalObjectHeader<O>::globalHeaderCounter;*/

	template<class O> TransactionalObjectHandle<O>* TransactionalObjectHeader<O>::createCreationHandle(Transaction* transaction) {
		TransactionalObjectHandle<O>* handle = new TransactionalObjectHandle<O>();
		handle->initialize(this, TransactionalObjectHandle<O>::CREATE, transaction);

		return handle;
	}

	template<class O> TransactionalObjectHandle<O>* TransactionalObjectHeader<O>::createReadOnlyHandle(Transaction* transaction) {
		TransactionalObjectHandle<O>* handle = new TransactionalObjectHandle<O>();
		handle->initialize(this, TransactionalObjectHandle<O>::READ, transaction);

		return handle;
	}

	template<class O> TransactionalObjectHandle<O>* TransactionalObjectHeader<O>::createWriteHandle(Transaction* transaction) {
		TransactionalObjectHandle<O>* handle = new TransactionalObjectHandle<O>();
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

	template<class O> O TransactionalObjectHeader<O>::getForDirty() {
		Transaction* trans = Transaction::currentTransaction();

		O obj = NULL;

		if (trans != NULL) {
			obj = trans->getOpenedObject<O>(this);
		}

		if (obj != NULL)
			return obj;
		else
			return transaction_cast<O>(object.get());
	}

	template<class O> Transaction* TransactionalObjectHeader<O>::acquireObject(Transaction* transaction) {
		/*System::out.println("[" + Thread::getCurrentThread()->getName() +"] acquiring (" + String::valueOf((uint64) ownerTransaction.get()) +
				", " + String::valueOf((uint64) transaction) + ")" );*/

		//Reference<Transaction*> str
		return ownerTransaction.compareAndSetReturnOld(NULL, transaction);
	}

	template<class O> void TransactionalObjectHeader<O>::discardObject(Transaction* transaction) {
		/*if (!ownerTransaction.compareAndSet(transaction, NULL))
			assert(0 && "hui");*/
		ownerTransaction.compareAndSet(transaction, NULL);
	}

	template <class O> Reference<TransactionalObjectHandle<O>*> TransactionalObjectHeader<O>::add(TransactionalObjectHandle<O>* handle) {
#ifdef STMLINKELISTDHELP
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
#else
		return NULL;
#endif
	}

	template<class O> bool TransactionalObjectHeader<O>::isCurrentVersion(Object* obj) {
		if (TransactionalObjectHeader<O>::ownerTransaction != NULL && TransactionalObjectHeader<O>::ownerTransaction != Transaction::currentTransaction())
			return false;

		return object == obj;
	}

	template<class O> void TransactionalObjectHeader<O>::releaseObject(TransactionalObjectHandle<O>* handle, Object* objectCopy) {
		//if (objectCopy != NULL) {

		//if (owner)

		//assert(ownerTransaction != NULL);

		Reference<Transaction*> oldTrans = ownerTransaction.get();

		if (oldTrans != NULL && ownerTransaction.compareAndSet(oldTrans.get(), NULL) && objectCopy != NULL) {
#ifdef MEMORY_PROTECTION
			object = objectCopy->clone(NULL);
#else
			object = objectCopy;
#endif
		}

		assert(object != NULL);

		//ownerTransaction.compareAndSet(ownerTransaction.get(), NULL);
		//ownerTransaction = NULL;
		//s}
	}

	template<class O> O TransactionalObjectHeader<O>::getObjectForRead(TransactionalObjectHandle<O>* handle) {
		Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

		assert(object != NULL);
#ifdef EAGERABORTS
		if (transaction != NULL) {
			if (!transaction->isCommited())
				return transaction_cast<O>(object.get());
			else
				throw TransactionAbortedException();

			//return ownerTransaction->getOpenedObject(this);
		} else {
			//add(handle);
			return transaction_cast<O>(object.get());
		}
#endif

		return transaction_cast<O>(object.get());
	}

	template<class O> O TransactionalObjectHeader<O>::getObjectForWrite(TransactionalObjectHandle<O>* handle) {
#ifdef EAGERABORTS
		Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

		if (transaction != NULL) {
			 	 if (!transaction->isCommited())
			 	 	 return transaction_cast<O>(object.get());
			 	 else
			 	 	 throw TransactionAbortedException();

			 	 //return ownerTransaction->getOpenedObject(this);
			 } else {
#endif

		this->add(handle);

		assert(object != NULL);

		O objectToReturn = transaction_cast<O>(object.get());

		assert(objectToReturn != NULL);
		return objectToReturn;

#ifdef EAGERABORTS
		}
#endif
	}

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTIONALOBJECTHEADER_H_ */
