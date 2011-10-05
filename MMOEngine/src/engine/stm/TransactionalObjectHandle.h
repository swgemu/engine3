/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_
#define ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_

#include "mm/KernelCall.h"

namespace engine {
  namespace stm {

	class Transaction;

	template<class O> class TransactionalObjectHeader;

	template<class O> class TransactionalObjectHandle : public Object {
		TransactionalObjectHeader<O>* header;

		Reference<O> object;
		O objectCopy;

		Reference<TransactionalObjectHandle<O>* > next;

		Reference<Transaction*> transaction;

	public:
		TransactionalObjectHandle();

		enum {CREATE, READ, WRITE};
		void initialize(TransactionalObjectHeader<O>* hdr, int accessType, Transaction* trans);

		virtual ~TransactionalObjectHandle();

		void upgradeToWrite();

		bool acquireHeader(Transaction* transaction);

		void releaseHeader();

		void discardHeader(Transaction* transaction);

		void setPrevious(TransactionalObjectHandle<O>* n);

		Transaction* getCompetingTransaction();

		TransactionalObjectHeader<O>* getObjectHeader() {
			return header;
		}

		Reference<TransactionalObjectHandle<Object*>*> getLastHandle() {
			Reference<TransactionalObjectHandle<Object*>*> ref = (TransactionalObjectHandle<Object*>*) header->getLastHandle().get();

			return ref;
		}

		bool hasObjectChanged();
		bool hasObjectContentChanged();

		int compareTo(TransactionalObjectHandle* handle) {
			if ((TransactionalObjectHandle*) this == handle)
				return 0;
			else if ((TransactionalObjectHandle*) this < handle)
				return 1;
			else
				return -1;
		}

		O getObject() {
			return object;
		}

		O getObjectLocalCopy() {
			return objectCopy;
		}

		Transaction* getTransaction() {
			return transaction;
		}

		inline void setTransaction(Transaction* trans) {
			transaction = trans;
		}

		inline void resetObjects() {
			object = NULL;

			if (objectCopy != NULL) {
				delete objectCopy;
				objectCopy = NULL;
			}
		}

		TransactionalObjectHandle<Object*>* getPrevious() {
			return (TransactionalObjectHandle<Object*>*) next;
		}
	};

	template<class O> TransactionalObjectHandle<O>::TransactionalObjectHandle() {
		header = NULL;
		transaction = NULL;

		object = NULL;

		objectCopy = NULL;
	}

	template<class O> void TransactionalObjectHandle<O>::initialize(TransactionalObjectHeader<O>* hdr, int accessType, Transaction* trans) {
		header = hdr;

		transaction = trans;

		if (accessType == WRITE) {
			//KernelCall kernelCall;

			//System::out.println("[" + Thread::getCurrentThread()->getName() +"] cloning " + String::valueOf((uint64) object));
			object = header->getObjectForWrite(this);

			objectCopy = dynamic_cast<O>(object->clone());

			//System::out.println("[" + Thread::getCurrentThread()->getName() +"] cloning " + String::valueOf((uint64) object) + " finished");
		} else if (accessType == READ){
			object = header->getObjectForRead(this);

			objectCopy = NULL;
		} else {
			object = header->getObjectForWrite(this);

			objectCopy = object;
		}
	}

	template<class O> TransactionalObjectHandle<O>::~TransactionalObjectHandle() {
		header = NULL;
		object = NULL;

		//assert(objectCopy == NULL);
	}

	template<class O> void TransactionalObjectHandle<O>::upgradeToWrite() {
		objectCopy = dynamic_cast<O>(object->clone());
	}

	template<class O> bool TransactionalObjectHandle<O>::acquireHeader(Transaction* transaction) {
		return header->acquireObject(transaction);
	}

	template<class O> void TransactionalObjectHandle<O>::releaseHeader() {
		header->releaseObject(this);

		objectCopy = NULL;
	}

	template<class O> void TransactionalObjectHandle<O>::discardHeader(Transaction* transaction) {
		header->discardObject(transaction);

		objectCopy = NULL;
	}

	template<class O> Transaction* TransactionalObjectHandle<O>::getCompetingTransaction() {
		return header->getTransaction();
	}

	template<class O> bool TransactionalObjectHandle<O>::hasObjectChanged() {
		return !header->hasObject(object);
	}

	template<class O> bool TransactionalObjectHandle<O>::hasObjectContentChanged() {
		return memcmp(object, objectCopy, sizeof(O)) != 0;
	}

	template<class O> void TransactionalObjectHandle<O>::setPrevious(TransactionalObjectHandle<O>* n) {
		next = n;
	}

  } // namespace stm
} // namespace engine


#endif /* ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_ */
