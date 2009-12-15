/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECT_H_
#define OBJECT_H_

#include "ref/ReferenceCounter.h"

#include "Variable.h"

#ifdef TRACE_REFERENCES
#include "ref/Reference.h"

#include "../util/VectorMap.h"

#include "../thread/Mutex.h"
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
	#ifdef TRACE_REFERENCES
		VectorMap<void*, StackTrace*> referenceHolders;

		Mutex refHoldersMutex;
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


	#ifdef TRACE_REFERENCES
		void addHolder(void* obj) {
			refHoldersMutex.lock();

			StackTrace* trace = new StackTrace();
			referenceHolders.put(obj, trace);

			refHoldersMutex.unlock();
		}

		void removeHolder(void* obj) {
			refHoldersMutex.lock();

			StackTrace* trace = referenceHolders.get(obj);

			if (trace != NULL) {
				delete trace;
				referenceHolders.drop(obj);
			}

			refHoldersMutex.unlock();
		}

		void printReferenceHolders() {
			for (int i = 0; i < referenceHolders.size(); ++i) {
				StackTrace* trace = referenceHolders.get(i);

				trace->print();
			}
		}
	#endif

		inline void acquire() {
			increaseCount();
		}

		inline void release() {
			if (decreaseCount())
				destroy();
		}

	protected:
		virtual void destroy() {
			delete this;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*OBJECT_H_*/
