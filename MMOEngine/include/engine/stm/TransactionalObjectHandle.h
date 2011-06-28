/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_
#define ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_

namespace engine {
  namespace stm {

	class Transaction;

	template<class O> class TransactionalObjectHeader;

	class TransactionalObjectHandleBase : public Object {
	public:
		virtual void releaseHeader() = 0;

		virtual void discardHeader(Transaction* transaction) = 0;

		virtual void resetObjects() = 0;

		virtual Transaction* getTransaction() = 0;

		virtual void setTransaction(Transaction* trans) = 0;

		virtual Reference<TransactionalObjectHandleBase*> getLastHandle() = 0;

		virtual TransactionalObjectHandleBase* getPrevious() = 0;

		virtual void setPrevious(TransactionalObjectHandleBase* n) = 0;

		virtual bool hasObjectChanged() = 0;
		virtual bool hasObjectContentChanged() = 0;

		virtual bool acquireHeader(Transaction* transaction) = 0;

		virtual Transaction* getCompetingTransaction() = 0;

		virtual Object* getObject() = 0;

		virtual Object* getObjectLocalCopy() = 0;
	};

	template<class O> class TransactionalObjectHandle : public TransactionalObjectHandleBase {
		TransactionalObjectHeader<O>* header;

		Reference<Object*> object;
		Reference<Object*> objectCopy;

		Reference<TransactionalObjectHandle<O>* > next;

		Reference<Transaction*> transaction;

	public:
		TransactionalObjectHandle();
		void initialize(TransactionalObjectHeader<O>* hdr, bool forWrite, Transaction* trans);

		virtual ~TransactionalObjectHandle();

		void upgradeToWrite();

		bool acquireHeader(Transaction* transaction);

		void releaseHeader();

		void discardHeader(Transaction* transaction);

		void setPrevious(TransactionalObjectHandleBase* n);

		Transaction* getCompetingTransaction();

		TransactionalObjectHeader<O>* getObjectHeader() {
			return header;
		}

		Reference<TransactionalObjectHandleBase*> getLastHandle() {
			Reference<TransactionalObjectHandleBase*> ref = header->getLastHandle().get();

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

		void resetObjects() {
			object = NULL;
			objectCopy = NULL;
		}

		Object* getObject() {
			return object;
		}

		Object* getObjectLocalCopy() {
			return objectCopy;
		}

		Transaction* getTransaction() {
			return transaction;
		}

		void setTransaction(Transaction* trans) {
			transaction = trans;
		}

		TransactionalObjectHandleBase* getPrevious() {
			return /*(TransactionalObjectHandle<Object*>*)*/ next.get();
		}
	};

	template<class O> TransactionalObjectHandle<O>::TransactionalObjectHandle() {
		header = NULL;
		transaction = NULL;

		object = NULL;

		objectCopy = NULL;
	}

	template<class O> void TransactionalObjectHandle<O>::initialize(TransactionalObjectHeader<O>* hdr, bool forWrite, Transaction* trans) {
		header = hdr;

		transaction = trans;


		if (forWrite) {
			//System::out.println("[" + Thread::getCurrentThread()->getName() +"] cloning " + String::valueOf((uint64) object));
			object = header->getObjectForWrite(this);

			objectCopy = object->clone();

			//System::out.println("[" + Thread::getCurrentThread()->getName() +"] cloning " + String::valueOf((uint64) object) + " finished");
		} else {
			object = header->getObjectForRead(this);

			objectCopy = NULL;
		}
	}

	template<class O> TransactionalObjectHandle<O>::~TransactionalObjectHandle() {
		header = NULL;
		object = NULL;

		//assert(objectCopy == NULL);
	}

	template<class O> void TransactionalObjectHandle<O>::upgradeToWrite() {
		objectCopy = object->clone();
	}

	template<class O> bool TransactionalObjectHandle<O>::acquireHeader(Transaction* transaction) {
		return header->acquireObject(transaction);
	}

	template<class O> void TransactionalObjectHandle<O>::releaseHeader() {
		header->releaseObject(this);

		objectCopy = NULL;
		object = NULL;
	}

	template<class O> void TransactionalObjectHandle<O>::discardHeader(Transaction* transaction) {
		header->discardObject(transaction);

		objectCopy = NULL;
		object = NULL;
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

	template<class O> void TransactionalObjectHandle<O>::setPrevious(TransactionalObjectHandleBase* n) {
		next = (TransactionalObjectHandle<O>*)n;
	}

  } // namespace stm
} // namespace engine


#endif /* ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_ */
