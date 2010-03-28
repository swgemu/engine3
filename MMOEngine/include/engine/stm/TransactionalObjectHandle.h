/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_
#define ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_

#include "TransactionalObject.h"

namespace engine {
  namespace stm {

	class Transaction;

	template<class O> class TransactionalObjectHeader;

	template<class O> class TransactionalObjectHandle {
		TransactionalObjectHeader<O>* header;

		O object;
		O objectCopy;

	public:
		TransactionalObjectHandle(TransactionalObjectHeader<O>* hdr);

		virtual ~TransactionalObjectHandle();

		bool acquireHeader(Transaction* transaction);

		void releaseHeader();

		bool discardHeader(Transaction* transaction);

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

		O getObject() {
			return object;
		}

		O getObjectLocalCopy() {
			return objectCopy;
		}
	};

	template<class O> TransactionalObjectHandle<O>::TransactionalObjectHandle(TransactionalObjectHeader<O>* hdr) {
		header = hdr;

		object = header->getObject();

		//System::out.println("[" + Thread::getCurrentThread()->getName() +"] cloning " + String::valueOf((uint64) object));

		objectCopy = (O) object->clone();
		//System::out.println("[" + Thread::getCurrentThread()->getName() +"] cloning " + String::valueOf((uint64) object) + " finished");
	}

	template<class O> TransactionalObjectHandle<O>::~TransactionalObjectHandle() {
		header = NULL;
		object = NULL;

		if (objectCopy != NULL) {
			//delete objectCopy;
			objectCopy = NULL;
		}
	}

	template<class O> bool TransactionalObjectHandle<O>::acquireHeader(Transaction* transaction) {
		return header->acquire(transaction);
	}

	template<class O> void TransactionalObjectHandle<O>::releaseHeader() {
		header->release(this);

		objectCopy = NULL;
	}

	template<class O> bool TransactionalObjectHandle<O>::discardHeader(Transaction* transaction) {
		return header->discard(transaction);
	}

	template<class O> Transaction* TransactionalObjectHandle<O>::getCompetingTransaction() {
		return header->getTransaction();
	}

	template<class O> bool TransactionalObjectHandle<O>::hasObjectChanged() {
		return object != header->getObject();
	}

	template<class O> bool TransactionalObjectHandle<O>::hasObjectContentChanged() {
		return memcmp(object, objectCopy, sizeof(O)) != 0;
	}

  } // namespace stm
} // namespace engine


#endif /* ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_ */
