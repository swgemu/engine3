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

	template<class O> class TransactionalObjectHandle {
		TransactionalObjectHeader<O>* header;

		Reference<Object*> object;
		Reference<Object*> objectCopy;

	public:
		TransactionalObjectHandle(TransactionalObjectHeader<O>* hdr);

		virtual ~TransactionalObjectHandle();

		bool acquireHeader(Transaction* transaction);

		void releaseHeader();

		void discardHeader(Transaction* transaction);

		Transaction* getCompetingTransaction();

		TransactionalObjectHeader<O>* getObjectHeader() {
			return header;
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

		Object* getObject() {
			return object;
		}

		Object* getObjectLocalCopy() {
			return objectCopy;
		}
	};

	template<class O> TransactionalObjectHandle<O>::TransactionalObjectHandle(TransactionalObjectHeader<O>* hdr) {
		header = hdr;

		object = header->getObject();

		//System::out.println("[" + Thread::getCurrentThread()->getName() +"] cloning " + String::valueOf((uint64) object));

		objectCopy = object->clone();
		//System::out.println("[" + Thread::getCurrentThread()->getName() +"] cloning " + String::valueOf((uint64) object) + " finished");
	}

	template<class O> TransactionalObjectHandle<O>::~TransactionalObjectHandle() {
		header = NULL;
		object = NULL;

		//assert(objectCopy == NULL);
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

  } // namespace stm
} // namespace engine


#endif /* ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_ */
