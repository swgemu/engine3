/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_
#define ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_

#include "mm/KernelCall.h"

#define transaction_cast dynamic_cast

namespace engine {
  namespace stm {

	class Transaction;

	template<class O> class TransactionalObjectHeader;

	class HandleCounter {
	public:
		static AtomicInteger createdHandles;
		static AtomicInteger deletedHandles;
	};

	class TransactionalObjectHandleBase : public Object {
	public:
		virtual Transaction* acquireHeader(Transaction* transaction) = 0;

		virtual void releaseHeader() = 0;

		virtual void discardHeader(Transaction* transaction) = 0;

		//virtual uint64 getHeaderID() = 0;

		virtual bool hasObjectChanged() = 0;

		virtual bool isCopyEqualToObject() = 0;

		virtual int compareTo(const TransactionalObjectHandleBase* handle) const = 0;

		virtual Transaction* getCompetingTransaction() = 0;

		virtual uint64 getHeaderAddress() = 0;

		virtual bool isWriteHandle() = 0;

		virtual bool isReadHandle() = 0;
	};

	template<class O> class TransactionalObjectHandle : public TransactionalObjectHandleBase {
		Reference<TransactionalObjectHeader<O>*> header;

		Reference<Object*> object;
		Reference<Object*> objectCopy;
		
		int currentType;

		//Reference<TransactionalObjectHandle<O>*> next;

	public:
		TransactionalObjectHandle();

		enum {CREATE, READ, WRITE};

		void initialize(TransactionalObjectHeader<O>* hdr, int accessType, Transaction* trans);

		virtual ~TransactionalObjectHandle();

		void upgradeToWrite();

		Transaction* acquireHeader(Transaction* transaction);

		void releaseHeader();

		void discardHeader(Transaction* transaction);

		void setPrevious(TransactionalObjectHandle<O>* n);

		Transaction* getCompetingTransaction();

		TransactionalObjectHeader<O>* getObjectHeader() {
			return header;
		}

		uint64 getHeaderAddress() {
			return (uint64)header.get();
		}

		Reference<TransactionalObjectHandle<Object*>*> getLastHandle() {
			Reference<TransactionalObjectHandle<Object*>*> ref = (TransactionalObjectHandle<Object*>*) header->getLastHandle().get();

			return ref;
		}

		bool isWriteHandle() {
			return currentType == WRITE || currentType == CREATE;
		}

		bool isReadHandle() {
			return currentType == READ;
		}

		bool hasObjectChanged();
		bool hasObjectContentChanged();

		bool isCopyEqualToObject() {
			return objectCopy == object;
		}

		int compareTo(Object* object) {
			TransactionalObjectHandle* otherHandle = (TransactionalObjectHandle*) object;

			return compareToHeaders(otherHandle);
		}

		int compareTo(const TransactionalObjectHandleBase* handle) const {
			/*if ((TransactionalObjectHandle*) this == handle)
				return 0;
			else if ((TransactionalObjectHandle*) this < handle)
				return 1;
			else
				return -1;*/
			return compareToHeaders((TransactionalObjectHandle*) handle);
		}

		int compareToHeaders(const TransactionalObjectHandle<O>* handle) const;

		//uint64 getHeaderID();

		Object* getObject() {
			return object;
		}

		Object* getObjectLocalCopy() {
			return objectCopy;
		}

		inline void resetObjects() {
			object = nullptr;

			objectCopy = nullptr;

			//header = nullptr;
		}
	};

	template<class O> TransactionalObjectHandle<O>::TransactionalObjectHandle() : header(nullptr), object(nullptr), objectCopy(nullptr) {
		currentType = 0;
	}

	template<class O> void TransactionalObjectHandle<O>::initialize(TransactionalObjectHeader<O>* hdr, int accessType, Transaction* trans) {
		header = hdr;

		assert((uintptr_t) trans > 0x1000);
		//transaction = trans;
		
		currentType = accessType;

		if (accessType == WRITE) {
			//KernelCall kernelCall;

			//System::out.println("[" + Thread::getCurrentThread()->getName() +"] cloning " + String::valueOf((uint64) object));
			object = header->getObjectForWrite(this);

			assert(object != nullptr);

			objectCopy = transaction_cast<O>(object->clone());

#ifdef MEMORY_PROTECTION
			ptrdiff_t rel = (ptrdiff_t)objectCopy.get() - (ptrdiff_t)0x8000000000;
			
			assert(!(rel > 0 && rel <= (ptrdiff_t) 0x7e800000));
#endif

			//System::out.println("[" + Thread::getCurrentThread()->getName() +"] cloning " + String::valueOf((uint64) object) + " finished");
		} else if (accessType == READ){
			object = header->getObjectForRead(this);
			
			assert(object != nullptr);

			objectCopy = nullptr;
		} else {
			object = header->getObjectForWrite(this);
			
			assert(object != nullptr);

			objectCopy = object;
			
#ifdef MEMORY_PROTECTION
			ptrdiff_t rel = (ptrdiff_t)objectCopy.get() - (ptrdiff_t)0x8000000000;
			
			assert(!(rel > 0 && rel <= (ptrdiff_t) 0x7e800000));
#endif
		}
	}

	template<class O> TransactionalObjectHandle<O>::~TransactionalObjectHandle() {
		header = nullptr;

		object = nullptr;
		objectCopy = nullptr;

	//	HandleCounter::deletedHandles.increment();

		/*printf("TransactionHandle %p deleted at\n", this);
		StackTrace::printStackTrace();*/
	}

	template<class O> void TransactionalObjectHandle<O>::upgradeToWrite() {
		header->add(this);

		objectCopy = object->clone();
		
		assert(objectCopy != nullptr);
		
		currentType = WRITE;

		//currentType = WRITE_AFTER_READ;
		/*
	        assert(object != nullptr);
	        
		objectCopy = static_cast<O>(object->clone());
		
		ptrdiff_t rel = (ptrdiff_t)objectCopy.get() - (ptrdiff_t)0x8000000000;
		
		assert(!(rel > 0 && rel <= (ptrdiff_t) 0x7e800000));*/
	}

	template<class O> Transaction* TransactionalObjectHandle<O>::acquireHeader(Transaction* transaction) {
		return header->acquireObject(transaction);
	}

	template<class O> void TransactionalObjectHandle<O>::releaseHeader() {
		Reference<Object*> obj = objectCopy.get();

		//if (objectCopy.compareAndSet(obj.get(), nullptr)) { // this is to avoid several threads releasing it*/

		//if (object.compareAndSet(object.get(), nullptr)) {
		header->releaseObject(this, obj);

		resetObjects();
		//}
		//}

	}

	template<class O> void TransactionalObjectHandle<O>::discardHeader(Transaction* transaction) {
		header->discardObject(transaction);

		resetObjects();

		//this->transaction = nullptr; <---------- this is wrong its the param
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

	/*template<class O> void TransactionalObjectHandle<O>::setPrevious(TransactionalObjectHandle<O>* n) {
		next = n;
	}*/

	/*template<class O> uint64 TransactionalObjectHandle<O>::getHeaderID() {
		return header->getHeaderID();
	}*/


	template<class O> int TransactionalObjectHandle<O>::compareToHeaders(const TransactionalObjectHandle<O>* handle) const {
		if (header == handle->header)
			return 0;
		else if (header < handle->header)
			return 1;
		else
			return -1;
	}

  } // namespace stm
} // namespace engine


#endif /* ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_ */
