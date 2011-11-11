/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECT_H_
#define OBJECT_H_

#include "system/lang/ref/ReferenceCounter.h"
//#include "system/lang/ref/WeakReference.h"

#include "system/thread/Mutex.h"

#include "system/util/ArrayList.h"

#include "system/thread/atomic/AtomicBoolean.h"

#include "Variable.h"

#ifdef TRACE_REFERENCES
#include "ref/Reference.h"

#include "system/util/VectorMap.h"
#endif

namespace engine {
        namespace stm {
                class TransactionalMemoryManager;
        }
}

namespace sys {
	namespace mm {
		class MemoryManager;
	}

	namespace io {
		class ObjectOutputStream;
		class ObjectInputStream;
	}

	namespace util {
		template<class O> class SortedVector;
		template<class E> class HashSet;
	}
}


namespace sys {
  namespace lang {

    class String;
    class WeakReferenceBase;
    template<class O> class WeakReference;

	using namespace sys::io;
	using namespace sys::util;

	class Object : public ReferenceCounter, public Variable {
#ifdef MEMORY_PROTECTION
		Mutex* referenceMutex;
#else
		Mutex referenceMutex;
#endif

		//ArrayList<WeakReferenceBase*> weakReferences;

//#ifndef WITH_STM
		HashSet<WeakReferenceBase*>* weakReferences;
/*#else
		TransactionalReference<Vector<WeakReferenceBase*>*>* weakReferences;
#endif*/
#ifdef MEMORY_PROTECTION
		AtomicBoolean* _destroying;
#else
		AtomicBoolean _destroying;
#endif

	#ifdef TRACE_REFERENCES
		VectorMap<void*, StackTrace*> referenceHolders;
	#endif

		//class StackTrace* deletedByTrace;

	public:
		Object();

		Object(const Object& obj);

		virtual ~Object();

		virtual Object* clone() {
			assert(0 && "clone method not declared");

			return NULL;
		}

		virtual Object* clone(void* object) {
			return clone();
		}

		virtual int compareTo(Object* object) {
			if (this == object)
				return 0;
			else if (this < object)
				return 1;
			else
				return -1;
		}


		virtual bool notifyDestroy() {
			return true;
		}

		bool _setDestroying() {
#ifdef MEMORY_PROTECTION
			return _destroying->compareAndSet(false, true);
#else
			return _destroying.compareAndSet(false, true);
#endif
		}

		void _clearDestroying() {
#ifdef MEMORY_PROTECTION
			_destroying->set(false);
#else
			_destroying.set(false);
#endif
		}

		void finalize() {
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			return false;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			return false;
		}

		inline bool _isGettingDestroyed() const {
#ifdef MEMORY_PROTECTION
			return _destroying->get();
#else
			return _destroying.get();
#endif
		}

		inline void acquire() {
			increaseCount();
		}

		void release();

		void acquireWeak(WeakReferenceBase* ref);

		void releaseWeak(WeakReferenceBase* ref);

		virtual String toString();

	#ifdef TRACE_REFERENCES
		void addHolder(void* obj);

		void removeHolder(void* obj);

		void printReferenceHolders();
	#endif

	protected:
		virtual void destroy();

		virtual void free();

		friend class sys::mm::MemoryManager;
		friend class WeakReference<Object*>;
		friend class engine::stm::TransactionalMemoryManager;
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*OBJECT_H_*/
