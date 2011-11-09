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

	template<class O> class TransactionalObjectHeader;

	namespace mm {
		template<class O> class TransactionalStrongObjectHeader;
		template<class O> class TransactionalWeakObjectHeader;
	}

	class TransactionalObjectMap : public HashTable<uint64, Reference<TransactionalObjectHandle<Object*>*> > {
	public:
		TransactionalObjectMap() : HashTable<uint64, Reference<TransactionalObjectHandle<Object*>*> >(1000) {
		}

		template<class O> TransactionalObjectHandle<O>* put(TransactionalObjectHeader<O>* header, TransactionalObjectHandle<O>* handle) {
			Reference<TransactionalObjectHandle<Object*>*> ref = (TransactionalObjectHandle<Object*>*) handle;

			return (TransactionalObjectHandle<O>*) HashTable<uint64, Reference<TransactionalObjectHandle<Object*>*> >::put((uint64) header, ref).get();
		}

		template<class O> TransactionalObjectHandle<O>* get(TransactionalObjectHeader<O>* header) {
			return (TransactionalObjectHandle<O>*) HashTable<uint64, Reference<TransactionalObjectHandle<Object*>*> >::get((uint64) header).get();
		}
	};

	class TransactionalObjectHandleVector : public SortedVector<Reference<TransactionalObjectHandle<Object*>*> > {
		int compare(Reference<TransactionalObjectHandle<Object*>*>& o1, const Reference<TransactionalObjectHandle<Object*>*>& o2) const {
			return o1->compareToHeaders(o2.get());
		}

	public:
		template<class O> void add(TransactionalObjectHandle<O>* handle) {
			SortedVector<Reference<TransactionalObjectHandle<Object*>*> >::put((TransactionalObjectHandle<Object*>*) handle);
		}

		template<class O> bool removeElement(TransactionalObjectHandle<O>* handle) {
			return SortedVector<Reference<TransactionalObjectHandle<Object*>*> >::drop((TransactionalObjectHandle<Object*>*) handle);
		}

		template<class O> bool contains(TransactionalObjectHandle<O>* handle) {
			return SortedVector<Reference<TransactionalObjectHandle<Object*>*> >::contains((TransactionalObjectHandle<Object*>*) handle);
		}
	};

	class TransactionalObjectHeaderMap : public HashTable<uint64, TransactionalObjectHeader<Object*>*> {
	public:
		TransactionalObjectHeaderMap() : HashTable<uint64, TransactionalObjectHeader<Object*>*>(1000) {
		}

		template<class O> TransactionalObjectHeader<O>* put(Object* object, TransactionalObjectHeader<O>* header) {
			return (TransactionalObjectHeader<O>*) HashTable<uint64, TransactionalObjectHeader<Object*>*>::put((uint64) object,
				dynamic_cast<TransactionalObjectHeader<Object*>*>(header));
		}

		template<class O> TransactionalObjectHeader<O>* get(O object) {
			return (TransactionalObjectHeader<O>*) HashTable<uint64, TransactionalObjectHeader<Object*>*>::get((uint64) dynamic_cast<O>(object));
		}
	};

	class Transaction : public Object, public Logger {
		uint64 tid;

		AtomicInteger status;

		TransactionalObjectMap openedObjets;

		TransactionalObjectHandleVector readOnlyObjects;
		TransactionalObjectHandleVector readWriteObjects;
		/*AtomicInteger currentWriteObjectAcquiring;
		AtomicInteger currentReadObjectAcquiring;
		AtomicInteger currentReadOnlyHandleCleaning;
		AtomicInteger currentReadWriteObjectCleanup;*/

		TransactionalObjectHeaderMap localObjectCache;

		Vector<Object*> reclaimedObjects;

		Reference<Task*> task;

		//SortedVector<Reference<Transaction*> > helpedTransactions;

		Vector<Command*> commands;

		uint64 commitTime;
		uint64 runTime;

	public:
		static ReadWriteLock blockLock;

		static const int UNDECIDED = 1;
		static const int READ_CHECKING = 2;
		static const int ABORTED = 3;
		static const int COMMITTED = 4;

		static const int INITIAL = 5;
	public:
		Transaction(uint64 id);

		virtual ~Transaction();

		bool start();
		bool start(Task* task);

		bool commit();

		int compareTo(Transaction* transaction);

		static Transaction* currentTransaction();

		String toString();

		inline bool isInitial() {
			return status == INITIAL;
		}

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

		uint64 getIdentifier() {
			return tid;
		}

		Task* getTask() {
			return task;
		}

	protected:
		template<class O> void createObject(TransactionalObjectHeader<O>* header);

		template<class O> O openObject(TransactionalObjectHeader<O>* header);

		template<class O> O openObjectForWrite(TransactionalObjectHeader<O>* header);

		//template<class O> O getOpenedObject(TransactionalObjectHeader<O>* header);

		template<class O> TransactionalObjectHeader<O>* getHeader(O object);

		bool doCommit();

		bool tryFinishCommit(int desiredStatus);

		void finishCommit();

		void doAbort();

		void resolveAbortedHandles();

		bool setState(int newstate);
		bool setState(int currentstate, int newstate);

		void releaseReadWriteObjects();

		bool validateReadOnlyObjects();

		void discardReadWriteObjects();

		void cleanReadOnlyHandles();

		void deleteObject(Object* object);

		Vector<Object*>* getDeletedObjects() {
			return &reclaimedObjects;
		}

		friend class TaskManager;
		friend class TransactionalMemoryManager;
		friend class TransactionalObjectManager;
		friend class TransactionAbortedException;
		template<class O> friend class TransactionalObjectHeader;
		template<class O> friend class TransactionalReference;
		template<class O> friend class TransactionalWeakReference;
	};

	template<class O> TransactionalObjectHeader<O>* Transaction::getHeader(O object) {
		TransactionalObjectHeader<O>* header = localObjectCache.get<O>(object);

		if (header == NULL) {
			return new engine::stm::TransactionalObjectHeader<O>(object);
		}

		return header;
	}

	template<class O> void Transaction::createObject(TransactionalObjectHeader<O>* header) {
		Reference<TransactionalObjectHandle<O>*> handle = header->createCreationHandle(this);

		localObjectCache.put(handle->getObjectLocalCopy(), header);
		localObjectCache.put(handle->getObject(), header);

		openedObjets.put<O>(header, handle);

		readWriteObjects.add<O>(handle);
	}

	template<class O> O Transaction::openObject(TransactionalObjectHeader<O>* header) {
		Reference<TransactionalObjectHandle<O>*> handle = openedObjets.get<O>(header);

		if (handle == NULL) {
			handle = header->createReadOnlyHandle(this);

			localObjectCache.put(handle->getObject(), header);

			openedObjets.put<O>(header, handle);

			readOnlyObjects.add<O>(handle);
		}

		O object = dynamic_cast<O>(handle->getObject());

		if (handle->getObjectLocalCopy() != NULL) {
			object = dynamic_cast<O>(handle->getObjectLocalCopy());
		}

		assert(object != NULL);
		return object;
	}

	template<class O> O Transaction::openObjectForWrite(TransactionalObjectHeader<O>* header) {
		//info("opening opbject");

		Reference<TransactionalObjectHandle<O>*> handle = openedObjets.get<O>(header);

		if (handle == NULL) {
			handle = header->createWriteHandle(this);

			localObjectCache.put(handle->getObjectLocalCopy(), header);
			localObjectCache.put(handle->getObject(), header);

			openedObjets.put<O>(header, handle);

			readWriteObjects.add<O>(handle);
		} else if (readOnlyObjects.contains(handle.get())) {
			readOnlyObjects.removeElement<O>(handle);

			readWriteObjects.add<O>(handle);
			handle->upgradeToWrite();

			localObjectCache.put(handle->getObjectLocalCopy(), header);
		}

		O localCopy = dynamic_cast<O>(handle->getObjectLocalCopy());

		assert(localCopy != NULL);
		
#ifdef MEMORY_PROTECTION
		ptrdiff_t rel = (ptrdiff_t)localCopy - (ptrdiff_t)0x8000000000;
		
		assert(!(rel > 0 && rel <= (ptrdiff_t) 0x7e800000));
#endif
		
		return localCopy;
	}

	/*template<class O> O Transaction::getOpenedObject(TransactionalObjectHeader<O>* header) {
		TransactionalObjectHandle<O>* handle = openedObjets.get<O>(header);

		//Reference<Transaction*> transaction = currentTransaction();
		Transaction* transaction = currentTransaction();

		if (status == COMMITTED)
			return dynamic_cast<O>(handle->getObjectLocalCopy());
		else
			return dynamic_cast<O>(handle->getObject());
	}*/

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTION_H_ */
