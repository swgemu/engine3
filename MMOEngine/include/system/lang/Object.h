/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECT_H_
#define OBJECT_H_

#include "ref/ReferenceCounter.h"

#include "ref/WeakReference.h"

#include "Variable.h"

#include "../thread/Mutex.h"

#include "../util/Vector.h"

#ifdef TRACE_REFERENCES
#include "ref/Reference.h"

#include "../util/VectorMap.h"
#endif

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
			//_references = obj._references;

			_destroying = false;
		}

		virtual ~Object() {
		#ifdef TRACE_REFERENCES
			for (int i = 0; i < referenceHolders.size(); ++i)
				delete referenceHolders.get(i);
		#endif

			finalize();
		}

		virtual bool notifyDestroy() {
			return true;
		}

		inline void _setDestroying(bool val) {
			_destroying = val;
		}

		void finalize() {
		}

		bool toString(String& str) {
			return false;
		}

		bool parseFromString(const String& str, int version = 0) {
			return false;
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
			if (decreaseCount()) {
				if (notifyDestroy())
					destroy();
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
