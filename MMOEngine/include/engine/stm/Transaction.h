/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_TRANSACTION_H_
#define ENGINE_STM_TRANSACTION_H_

#include "system/lang.h"

#include "TransactionalObjectHandle.h"

namespace engine {
  namespace stm {

	template<class O> class TransactionalObjectHeader;

	class TransactionalObjectMap : public HashTable<uint64, TransactionalObjectHandle<TransactionalObject>*> {
		int hash(const uint64& key) {
			return Long::hashCode(key);
		}

	public:
		TransactionalObjectMap() : HashTable<uint64, TransactionalObjectHandle<TransactionalObject>*>(1000) {
			setNullValue(NULL);
		}
	};

	class Transaction {
		AtomicInteger status;

		TransactionalObjectMap openedObjets;

		SortedVector<TransactionalObjectHandle<TransactionalObject>*> readOnlyObjects;
		SortedVector<TransactionalObjectHandle<TransactionalObject>*> readWriteObjects;

		static const int UNDECIDED = 0;
		static const int READ_CHECKING = 1;
		static const int ABORTED = 2;
		static const int COMMITTED = 3;

	public:
		Transaction() {
			status = UNDECIDED;
		}

		virtual ~Transaction();

		bool commit();

		void abort();

		void reset();

		bool acquireReadWriteObjects();

		void releaseReadWriteObjects();

		bool validateReadOnlyObjects();

		bool resolveConflict(Transaction* transaction);

		void discardReadWriteObjects();

		template<class O> O* openObject(TransactionalObjectHeader<O>* header);

		template<class O> O* openObjectForWrite(TransactionalObjectHeader<O>* header);

		inline bool isUndecided() {
			return status == UNDECIDED;
		}

		inline bool isReadChecking() {
			return status == READ_CHECKING;
		}

		inline bool isAborted() {
			return status == ABORTED;
		}

		inline bool isCommited() {
			return status == COMMITTED;
		}

		static Transaction* currentTransaction();

	protected:
		bool setState(int currentstate, int newstate);
	};

	template<class O> O* Transaction::openObject(TransactionalObjectHeader<O>* header) {
		TransactionalObjectHandle<TransactionalObject>* handle = openedObjets.get((uint64) header);

		if (handle == NULL) {
			handle = (TransactionalObjectHandle<TransactionalObject>*) header->createHandle();

			openedObjets.put((uint64) header, handle);

			readOnlyObjects.add(handle);
		}

		return (O*) handle->getObjectLocalCopy();
	}

	template<class O> O* Transaction::openObjectForWrite(TransactionalObjectHeader<O>* header) {
		TransactionalObjectHandle<TransactionalObject>* handle = openedObjets.get((uint64) header);

		if (handle == NULL) {
			handle = (TransactionalObjectHandle<TransactionalObject>*)  header->createHandle();

			openedObjets.put((uint64) header, handle);

			readWriteObjects.add(handle);
		} else if (readOnlyObjects.contains(handle)) {
			readOnlyObjects.removeElement(handle);

			readWriteObjects.add(handle);
		}

		return (O*) handle->getObjectLocalCopy();
	}

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTION_H_ */
