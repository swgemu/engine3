/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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

  	class STMAlgorithm;

	template<class O> class TransactionalObjectHeader;

	namespace mm {
		template<class O> class TransactionalStrongObjectHeader;
		template<class O> class TransactionalWeakObjectHeader;
	}

	class TransactionalObjectMap : public HashTable<uint64, Reference<TransactionalObjectHandleBase*> > {
	public:
		TransactionalObjectMap() : HashTable<uint64, Reference<TransactionalObjectHandleBase*> >(1000) {
		}

		template<class O> TransactionalObjectHandle<O>* put(TransactionalObjectHeader<O>* header, TransactionalObjectHandle<O>* handle) {
			return transaction_cast<TransactionalObjectHandle<O>*>(HashTable<uint64, Reference<TransactionalObjectHandleBase*> >::put((uint64) header, handle).get());
		}

		template<class O> TransactionalObjectHandle<O>* get(TransactionalObjectHeader<O>* header) {
			return transaction_cast<TransactionalObjectHandle<O>*>(HashTable<uint64, Reference<TransactionalObjectHandleBase*> >::get((uint64) header).get());
		}
	};

	class TransactionalObjectHandleVector : public SortedVector<TransactionalObjectHandleBase* > {
		int compare(TransactionalObjectHandleBase* const& o1, TransactionalObjectHandleBase* const& o2) const override {
			//this needs a specific order based on the header to not create cycle helps
			return o1->compareTo(o2);
		}

	public:
		TransactionalObjectHandleVector() {
			setInsertPlan(NO_DUPLICATE);
		}

		template<class O> void addHandle(TransactionalObjectHandle<O>* handle) {
			SortedVector<TransactionalObjectHandleBase* >::put(handle);
		}

		template<class O> bool removeHandle(TransactionalObjectHandle<O>* handle) {
			return SortedVector<TransactionalObjectHandleBase* >::drop(handle);
		}

		template<class O> bool containsHandle(TransactionalObjectHandle<O>* handle) {
			return SortedVector<TransactionalObjectHandleBase* >::contains(handle);
		}
	};

	class TransactionalObjectHeaderMap : public HashTable<uint64, Reference<TransactionalObjectHeader<Object*>*> > {
	public:
		TransactionalObjectHeaderMap() : HashTable<uint64, Reference<TransactionalObjectHeader<Object*>*> >(1000) {
		}

		template<class O> TransactionalObjectHeader<O>* put(Object* object, TransactionalObjectHeader<O>* header) {

			TransactionalObjectHeader<Object*>* test = (TransactionalObjectHeader<Object*>*)header;

			//assert(test != nullptr);

			return (TransactionalObjectHeader<O>*) HashTable<uint64, Reference<TransactionalObjectHeader<Object*>*> >::put((uint64) object,
				test).get();
		}

		template<class O> TransactionalObjectHeader<O>* get(O object) {
			return (TransactionalObjectHeader<O>*) HashTable<uint64, Reference<TransactionalObjectHeader<Object*>*> >::get((uint64) (object)).get();
		}
	};

	class Transaction : public Object, public Logger {
		uint64 tid;

		AtomicInteger status;

		TransactionalObjectMap openedObjets;

		TransactionalObjectHandleVector readOnlyObjects;
		TransactionalObjectHandleVector readWriteObjects;
		TransactionalObjectHeaderMap localObjectCache;

		Vector<Object*> reclaimedObjects;

		Reference<Task*> task;

		Vector<Command*> commands;

		uint64 commitTime;
		uint64 runTime;

		static UniqueReference<STMAlgorithm*> commitAlgorithm;

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

		inline bool isInitial() const {
			return status == INITIAL;
		}

		inline bool isUndecided() const {
			return status == UNDECIDED;
		}

		inline bool isReadChecking() const {
			return status == READ_CHECKING;
		}

		inline bool isAborted() const {
			return status == ABORTED;
		}

		inline bool isCommited() const {
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

		template<class O> O getOpenedObject(TransactionalObjectHeader<O>* header);

		template<class O> TransactionalObjectHeader<O>* getHeader(O object);

		bool doCommit();

		void finishCommit();

		void doAbort();

		void resolveAbortedHandles();

		bool setState(int newstate);
		bool setState(int currentstate, int newstate);

		void releaseReadWriteObjects();

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
		friend class FraserSTM;
		template<class O> friend class TransactionalObjectHeader;
		template<class O> friend class TransactionalReference;
		template<class O> friend class TransactionalWeakReference;
	};

	template<class O> TransactionalObjectHeader<O>* Transaction::getHeader(O object) {
		TransactionalObjectHeader<O>* header = localObjectCache.get<O>(object);

		if (header == nullptr) {
			return new engine::stm::TransactionalObjectHeader<O>(object);
		}

		return header;
	}

	template<class O> void Transaction::createObject(TransactionalObjectHeader<O>* header) {
		assert(!isCommited());

		TransactionalObjectHandle<O>* handle = header->createCreationHandle(this);

		openedObjets.put<O>(header, handle);

		localObjectCache.put(handle->getObjectLocalCopy(), header);
		localObjectCache.put(handle->getObject(), header);

		readWriteObjects.addHandle<O>(handle);
	}

	template<class O> O Transaction::getOpenedObject(TransactionalObjectHeader<O>* header) {
		TransactionalObjectHandle<O>* handle = openedObjets.get<O>(header);

		if (handle == nullptr)
			return nullptr;

		O object = nullptr;

		if (!isAborted()) {
			object = transaction_cast<O>(handle->getObject());

			if (handle->getObjectLocalCopy() != nullptr) {
				object = transaction_cast<O>(handle->getObjectLocalCopy());
			}
		}

		return object;
	}

	template<class O> O Transaction::openObject(TransactionalObjectHeader<O>* header) {
		assert(!isCommited());

		TransactionalObjectHandle<O>* handle = openedObjets.get<O>(header);

		if (handle == nullptr) {
			handle = header->createReadOnlyHandle(this);

			openedObjets.put<O>(header, handle);

			localObjectCache.put(handle->getObject(), header);

			readOnlyObjects.addHandle<O>(handle);
		}

		O object = transaction_cast<O>(handle->getObject());

		if (handle->getObjectLocalCopy() != nullptr) {
			object = transaction_cast<O>(handle->getObjectLocalCopy());
		}

		assert(object != nullptr);
		return object;
	}

	template<class O> O Transaction::openObjectForWrite(TransactionalObjectHeader<O>* header) {
		//info("opening opbject");
		assert(!isCommited());

		TransactionalObjectHandle<O>* handle = openedObjets.get<O>(header);

		if (handle == nullptr) {
			handle = header->createWriteHandle(this);

			openedObjets.put<O>(header, handle);

			localObjectCache.put(handle->getObjectLocalCopy(), header);
			localObjectCache.put(handle->getObject(), header);

			readWriteObjects.addHandle<O>(handle);
		} else if (readOnlyObjects.containsHandle(handle)) {
			readOnlyObjects.removeHandle<O>(handle);

			readWriteObjects.addHandle<O>(handle);
			handle->upgradeToWrite();

			localObjectCache.put(handle->getObjectLocalCopy(), header);
		}

		O localCopy = transaction_cast<O>(handle->getObjectLocalCopy());

		assert(localCopy != nullptr);

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
			return static_cast<O>(handle->getObjectLocalCopy());
		else
			return static_cast<O>(handle->getObject());
	}*/

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTION_H_ */
