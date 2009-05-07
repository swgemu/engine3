/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECT_H_
#define OBJECT_H_

#include "ref/ReferenceCounter.h"

#include "Variable.h"

#ifdef TRACE_REFERENCESLOTS
#include "ref/ReferenceSlot.h"
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
		bool finalized;

	#ifdef TRACE_REFERENCESLOTS
		VectorMap<void*, StackTrace*> referenceHolders;

		Mutex refHoldersMutex;
	#endif

	public:
		Object() : ReferenceCounter(), Variable() {
			initializeCount();

		#ifdef TRACE_REFERENCESLOTS
			referenceHolders.setNullValue(NULL);
		#endif

			finalized = false;
		}

		Object(const Object& obj) : ReferenceCounter(), Variable() {
			_references = obj._references;
			finalized = obj.finalized;
		}

		virtual ~Object() {
		#ifdef TRACE_REFERENCESLOTS
			for (int i = 0; i < referenceHolders.size(); ++i)
				delete referenceHolders.get(i);
		#endif
		}

		virtual void finalize() {
			if (finalized)
				return;

			finalized = true;

			release();
		}

		void revoke() {
			if (!finalized)
				return;

			finalized = false;

			acquire();
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


	#ifdef TRACE_REFERENCESLOTS
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
		virtual bool destroy() {
			if (finalized) {
				delete this;

				return true;
			} else
				return false;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*OBJECT_H_*/
