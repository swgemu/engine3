/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECT_H_
#define OBJECT_H_

#include "ref/ReferenceCounter.h"

#include "ref/WeakReference.h"

#include "Variable.h"

#include "system/thread/Mutex.h"

#include "system/util/Vector.h"

#ifdef TRACE_REFERENCES
#include "ref/Reference.h"

#include "system/util/VectorMap.h"
#endif

#include "system/util/VectorMap.h"

namespace sys {
	namespace io {
		class ObjectOutputStream;
		class ObjectInputStream;
	}
}

namespace sys {
  namespace lang {

    class String;

	using namespace sys::io;

	class Object : public ReferenceCounter, public Variable {
		Mutex referenceMutex;

		Vector<WeakReferenceBase*> weakReferences;

		bool _destroying;

		static VectorMap<void*, StackTrace*> createHolders;
		static VectorMap<void*, StackTrace*> deleteHolders;

	#ifdef TRACE_REFERENCES
		VectorMap<void*, StackTrace*> referenceHolders;
	#endif

	public:
		Object() : ReferenceCounter(), Variable() {
			_destroying = false;

		#ifdef TRACE_REFERENCES
			referenceHolders.setNullValue(NULL);
		#endif
		}

		Object(const Object& obj) : ReferenceCounter(), Variable() {
			_destroying = false;

		#ifdef TRACE_REFERENCES
			referenceHolders.setNullValue(NULL);
		#endif
		}

		virtual ~Object() {
		#ifdef TRACE_REFERENCES
			for (int i = 0; i < referenceHolders.size(); ++i)
				delete referenceHolders.get(i);
		#endif
			finalize();
		}

		virtual Object* clone() {
			assert(0 && "clone method not declared");
		}

		virtual bool notifyDestroy() {
			return true;
		}

		inline void _setDestroying(bool val) {
			_destroying = val;
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
			return _destroying;
		}

		inline void acquire() {
			increaseCount();
		}

		inline void release() {
			if (_references.get() == 0) {
				printf("Object already delted by\n");

				//deleteHolders.get((void*) this)->print();
			}

			//deleteHolders.put(this, new StackTrace());
			//assert(deleteHolders.get((void*) this) != NULL);

			if (decreaseCount()) {
				if (notifyDestroy()) {
					destroy();
				}
			}
		}

		void acquireWeak(WeakReferenceBase* ref);

		void releaseWeak(WeakReferenceBase* ref);

	#ifdef TRACE_REFERENCES
		void addHolder(void* obj);

		void removeHolder(void* obj);

		void printReferenceHolders();
	#endif

	protected:
		virtual void destroy();

		friend class WeakReference<Object*>;
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*OBJECT_H_*/
