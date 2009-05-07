/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef REFERENCESLOT_H_
#define REFERENCESLOT_H_

#include "../Object.h"

namespace sys {
  namespace lang {

	template<class O> class ReferenceSlot : public Variable {
	protected:
		O* object;

	public:
		ReferenceSlot() : Variable() {
			object = NULL;
		}

		ReferenceSlot(const ReferenceSlot& refslot) : Variable() {
			initializeObject(refslot.object);
		}

		ReferenceSlot(O* obj) : Variable() {
			initializeObject(obj);
		}

		virtual ~ReferenceSlot() {
			releaseObject();
		}

		void operator=(const ReferenceSlot& refslot) {
			setObject(refslot.object);
		}

		void operator=(O* obj) {
			updateObject(obj);
		}

		O* operator->() {
			return object;
		}

		operator O*() {
			return object;
		}

		inline O* get() {
			return object;
		}

		bool toString(String& str) {
			return object->toString(str);
		}

		bool parseFromString(const String& str, int version = 0) {
			return object->parseFromString(str, version);
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			return object->toBinaryStream(stream);
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			return object->parseFromBinaryStream(stream);
		}

	protected:
		inline void updateObject(O* obj) {
			if (obj == object)
				return;

			releaseObject();

			setObject(obj);
		}

		inline void setObject(O* obj) {
			if (obj == object)
				return;

			initializeObject(obj);
		}

		inline void initializeObject(O* obj) {
			object = obj;

			acquireObject();
		}

		inline void acquireObject() {
			if (object != NULL) {
			#ifdef TRACE_REFERENCESLOTS
				object->addHolder(this);
			#endif
				object->acquire();
			}
		}

		inline void releaseObject() {
			if (object != NULL) {
			#ifdef TRACE_REFERENCESLOTS
				object->removeHolder(this);
			#endif
				object->release();
				object = NULL;
			}
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*REFERENCESLOT_H_*/
