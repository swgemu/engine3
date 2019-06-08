/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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
		TransactionalObjectHeader(O obj) : ownerTransaction(nullptr), object(obj) {
			createObject();

			assert(object != nullptr);
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
			return add(nullptr);
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

		O obj = nullptr;

		if (trans != nullptr) {
			obj = trans->getOpenedObject<O>(this);
		}

		if (obj != nullptr)
			return obj;
		else
			return transaction_cast<O>(object.get());
	}

	template<class O> Transaction* TransactionalObjectHeader<O>::acquireObject(Transaction* transaction) {
		/*System::out.println("[" + Thread::getCurrentThread()->getName() +"] acquiring (" + String::valueOf((uint64) ownerTransaction.get()) +
				", " + String::valueOf((uint64) transaction) + ")" );*/

		//Reference<Transaction*> str
		return ownerTransaction.compareAndSetReturnOld(nullptr, transaction);
	}

	template<class O> void TransactionalObjectHeader<O>::discardObject(Transaction* transaction) {
		/*if (!ownerTransaction.compareAndSet(transaction, nullptr))
			assert(0 && "hui");*/
		ownerTransaction.compareAndSet(transaction, nullptr);
	}

	template <class O> Reference<TransactionalObjectHandle<O>*> TransactionalObjectHeader<O>::add(TransactionalObjectHandle<O>* handle) {
#ifdef STMLINKELISTDHELP
		if (handle != nullptr)
			handle->acquire();

		while (true) {
			TransactionalObjectHandle<O>* lastRef = last.get();

			if (last.compareAndSet(lastRef, handle)) {
				if (lastRef != nullptr && handle != nullptr) {
					lastRef->setPrevious(handle);
				}

				Reference<TransactionalObjectHandle<O>*> strong = lastRef;

				if (lastRef != nullptr)
					lastRef->release();

				return strong;
			}

			Thread::yield();
		}
#else
		return nullptr;
#endif
	}

	template<class O> bool TransactionalObjectHeader<O>::isCurrentVersion(Object* obj) {
		if (TransactionalObjectHeader<O>::ownerTransaction != nullptr && TransactionalObjectHeader<O>::ownerTransaction != Transaction::currentTransaction())
			return false;

		return object == obj;
	}

	template<class O> void TransactionalObjectHeader<O>::releaseObject(TransactionalObjectHandle<O>* handle, Object* objectCopy) {
		//if (objectCopy != nullptr) {

		//if (owner)

		//assert(ownerTransaction != nullptr);

		Reference<Transaction*> oldTrans = ownerTransaction.get();

		if (oldTrans != nullptr && ownerTransaction.compareAndSet(oldTrans.get(), nullptr) && objectCopy != nullptr) {
#ifdef MEMORY_PROTECTION
			object = objectCopy->clone(nullptr);
#else
			object = objectCopy;
#endif
		}

		assert(object != nullptr);

		//ownerTransaction.compareAndSet(ownerTransaction.get(), nullptr);
		//ownerTransaction = nullptr;
		//s}
	}

	template<class O> O TransactionalObjectHeader<O>::getObjectForRead(TransactionalObjectHandle<O>* handle) {
		Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

		assert(object != nullptr);
#ifdef EAGERABORTS
		if (transaction != nullptr) {
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

		if (transaction != nullptr) {
			 	 if (!transaction->isCommited())
			 	 	 return transaction_cast<O>(object.get());
			 	 else
			 	 	 throw TransactionAbortedException();

			 	 //return ownerTransaction->getOpenedObject(this);
			 } else {
#endif

		this->add(handle);

		assert(object != nullptr);

		O objectToReturn = transaction_cast<O>(object.get());

		assert(objectToReturn != nullptr);
		return objectToReturn;

#ifdef EAGERABORTS
		}
#endif
	}

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTIONALOBJECTHEADER_H_ */
