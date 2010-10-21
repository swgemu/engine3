/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_TRANSACTION_H_
#define ENGINE_STM_TRANSACTION_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "engine/core/TaskQueue.h"
#include "engine/core/TaskScheduler.h"

#include "engine/util/Command.h"

#include "TransactionalObjectHandle.h"

namespace engine {
  namespace stm {

	class TransactionalObject;
	template<class O> class TransactionalObjectHeader;

	class TransactionalObjectMap : public HashTable<uint64, TransactionalObjectHandle<TransactionalObject*>*> {
		int hash(const uint64& key) {
			return Long::hashCode(key);
		}

	public:
		TransactionalObjectMap() : HashTable<uint64, TransactionalObjectHandle<TransactionalObject*>*>(1000) {
			setNullValue(NULL);
		}

		template<class O> TransactionalObjectHandle<O>* put(TransactionalObjectHeader<O>* header, TransactionalObjectHandle<O>* handle) {
			return (TransactionalObjectHandle<O>*) HashTable<uint64, TransactionalObjectHandle<TransactionalObject*>*>::put((uint64) header,
				(TransactionalObjectHandle<TransactionalObject*>*) handle);
		}

		template<class O> TransactionalObjectHandle<O>* get(TransactionalObjectHeader<O>* header) {
			return (TransactionalObjectHandle<O>*) HashTable<uint64, TransactionalObjectHandle<TransactionalObject*>*>::get((uint64) header);
		}
	};

	class TransactionalObjectHandleVector : public SortedVector<TransactionalObjectHandle<TransactionalObject*>*> {
	public:
		template<class O> void add(TransactionalObjectHandle<O>* handle) {
			SortedVector<TransactionalObjectHandle<TransactionalObject*>*>::add((TransactionalObjectHandle<TransactionalObject*>*) handle);
		}

		template<class O> bool removeElement(TransactionalObjectHandle<O>* handle) {
			return SortedVector<TransactionalObjectHandle<TransactionalObject*>*>::removeElement((TransactionalObjectHandle<TransactionalObject*>*) handle);
		}

		template<class O> bool contains(TransactionalObjectHandle<O>* handle) {
			return SortedVector<TransactionalObjectHandle<TransactionalObject*>*>::contains((TransactionalObjectHandle<TransactionalObject*>*) handle);
		}
	};

	class Transaction : public Logger {
		AtomicInteger status;

		TransactionalObjectMap openedObjets;

		TransactionalObjectHandleVector readOnlyObjects;
		TransactionalObjectHandleVector readWriteObjects;

		uint64 commitTime;
		int commitAttempts;

		Vector<Command*> commands;

		static const int UNDECIDED = 0;
		static const int READ_CHECKING = 1;
		static const int ABORTED = 2;
		static const int COMMITTED = 3;

	public:
		Transaction();

		virtual ~Transaction();

		bool commit();

		void abort();

		template<class O> O openObject(TransactionalObjectHeader<O>* header);

		template<class O> O openObjectForWrite(TransactionalObjectHeader<O>* header);

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

		String toString();

	protected:
		bool doCommit();

		bool setState(int currentstate, int newstate);

		void reset();

		bool acquireReadWriteObjects();

		void releaseReadWriteObjects();

		bool validateReadOnlyObjects();

		bool resolveConflict(Transaction* transaction);

		void discardReadWriteObjects();

		friend class TaskManager;
	};

	template<class O> O Transaction::openObject(TransactionalObjectHeader<O>* header) {
		TransactionalObjectHandle<O>* handle = openedObjets.get<O>(header);

		if (handle == NULL) {
			handle = header->createHandle();

			openedObjets.put<O>(header, handle);

			readOnlyObjects.add<O>(handle);
		}

		O localCopy = (O) handle->getObjectLocalCopy();

		return localCopy;
	}

	template<class O> O Transaction::openObjectForWrite(TransactionalObjectHeader<O>* header) {
		TransactionalObjectHandle<O>* handle = openedObjets.get<O>(header);

		if (handle == NULL) {
			handle = header->createHandle();

			openedObjets.put<O>(header, handle);

			readWriteObjects.add<O>(handle);
		} else if (readOnlyObjects.contains(handle)) {
			readOnlyObjects.removeElement<O>(handle);

			readWriteObjects.add<O>(handle);
		}

		O localCopy = (O) handle->getObjectLocalCopy();

		return localCopy;
	}

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTION_H_ */
