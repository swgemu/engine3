/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECT_H_
#define OBJECT_H_

#include "ref/ReferenceCounter.h"

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

    class WeakReference;

	using namespace sys::io;

	class Object : public ReferenceCounter, public Variable {
		Mutex referenceMutex;

		Vector<WeakReference*> weakReferences;

	#ifdef TRACE_REFERENCES
		VectorMap<void*, StackTrace*> referenceHolders;
	#endif

	public:
		Object() : ReferenceCounter(), Variable() {
			initializeCount();

		#ifdef TRACE_REFERENCES
			referenceHolders.setNullValue(NULL);
		#endif
		}

		Object(const Object& obj) : ReferenceCounter(), Variable() {
			_references = obj._references;
		}

		virtual ~Object() {
		#ifdef TRACE_REFERENCES
			for (int i = 0; i < referenceHolders.size(); ++i)
				delete referenceHolders.get(i);
		#endif

			finalize();
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

		inline void acquire() {
			increaseCount();
		}

		inline void release() {
			if (decreaseCount())
				destroy();
		}

		void acquireWeak(WeakReference* ref);

		void releaseWeak(WeakReference* ref);

	#ifdef TRACE_REFERENCES
		void addHolder(void* obj);

		void removeHolder(void* obj);

		void printReferenceHolders();
	#endif

	protected:
		virtual void destroy();

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*OBJECT_H_*/
