/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECT_H_
#define OBJECT_H_

#include "system/lang/ref/ReferenceCounter.h"
#include "system/lang/ref/WeakReference.h"

#include "system/thread/Mutex.h"

#include "system/util/ArrayList.h"

#include "system/thread/atomic/AtomicBoolean.h"

#include "Variable.h"

#ifdef TRACE_REFERENCES
#include "ref/Reference.h"

#include "system/util/VectorMap.h"
#endif

namespace sys {
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

	using namespace sys::io;
	using namespace sys::util;

	class Object : public ReferenceCounter, public Variable {
//	#ifndef WITH_STM
		Mutex referenceMutex;
//	#endif

		//ArrayList<WeakReferenceBase*> weakReferences;

//#ifndef WITH_STM
		HashSet<WeakReferenceBase*>* weakReferences;
/*#else
		TransactionalReference<Vector<WeakReferenceBase*>*>* weakReferences;
#endif*/

		AtomicBoolean _destroying;

	#ifdef TRACE_REFERENCES
		VectorMap<void*, StackTrace*> referenceHolders;
	#endif

	public:
		Object();

		Object(const Object& obj);

		virtual ~Object();

		virtual Object* clone() {
			assert(0 && "clone method not declared");

			return NULL;
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
			return _destroying.compareAndSet(false, true);
		}

		void _clearDestroying() {
			_destroying.set(false);
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
			return _destroying.get();
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

		friend class MemoryManager;
		friend class WeakReference<Object*>;
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*OBJECT_H_*/
