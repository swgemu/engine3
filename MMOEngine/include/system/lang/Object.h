/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECT_H_
#define OBJECT_H_

#include "ref/ReferenceCounter.h"

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

	class Object : public ReferenceCounter {
		bool finalized;

	#ifdef TRACE_REFERENCESLOTS
		VectorMap<void*, StackTrace*> referenceHolders;

		Mutex refHoldersMutex;
	#endif

	public:
		Object() : ReferenceCounter() {
			initializeCount();

		#ifdef TRACE_REFERENCESLOTS
			referenceHolders.setNullValue(NULL);
		#endif

			finalized = false;
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

		/*virtual void toString(String* str) {

		}

		virtual void parseFromString(String* str) {

		}

		virtual bool parseFromString(String* str, int version) {
			return false;
		}

		virtual void toBinaryStream(ObjectOutputStream* stream) {

		}

		virtual void parseFromBinaryStream(ObjectInputStream* stream) {

		}*/


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

	protected:
		inline void acquire() {
			increaseCount();
		}

		inline void release() {
			if (decreaseCount())
				destroy();
		}

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
