/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef REFERENCE_H_
#define REFERENCE_H_

#include "../Object.h"
#include "../Long.h"
#include "system/thread/atomic/AtomicReference.h"

namespace sys {
  namespace lang {

	template<class O> class Reference : public Variable {
	protected:
		AtomicReference<O> object;

	public:
		Reference() : Variable() {
			object = NULL;
		}

		Reference(const Reference& ref) : Variable() {
			initializeObject(ref.object.get());
		}

		Reference(O obj) : Variable() {
			initializeObject(obj);
		}

		virtual ~Reference() {
			releaseObject();
		}

		virtual int compareTo(const Reference& val) const {
			if (object < val.object)
				return 1;
			else if (object > val.object)
				return -1;
			else
				return 0;
		}

		Reference& operator=(const Reference& ref) {
			if (this == &ref)
				return *this;

			updateObject(ref.object.get());

			return *this;
		}

		int hashCode() const {
			return UnsignedLong::hashCode((uint64)object.get());
		}

		O operator=(O obj) {
			updateObject(obj);

			return object.get();
		}

		bool operator==(O obj) {
			return object.get() == obj;
		}

		bool operator!=(O obj) {
			return object.get() != obj;
		}

		O operator->() const {
			return object.get();
		}

		operator O() const {
			return object.get();
		}

		inline O get() const {
			return object.get();
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			//return object->toBinaryStream(stream);
			return false;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			//return object->parseFromBinaryStream(stream);
			return false;
		}

	protected:
		inline void updateObject(O obj) {
			if (obj == object.get())
				return;

			//This needs to be an atomic operation, 2 threads updating/reading this messes shit up
			//Thread A reading while thread B updating, thread A reads NULL cause it releases and then acquires
			/*releaseObject();

			setObject(obj);*/

			if (obj != NULL)
				((Object*)obj)->acquire();

			while (true) {
				O oldobj = object.get();

				if (object.compareAndSet(oldobj, obj)) {
					if (oldobj != NULL)
						((Object*)oldobj)->release();

					return;
				}
			}

		}

		inline void setObject(O obj) {
			if (obj == object)
				return;

			initializeObject(obj);
		}

		inline void initializeObject(O obj) {
			object = obj;

			acquireObject();
		}

		inline void acquireObject() {
			if (object != NULL) {
			#ifdef TRACE_REFERENCES
				object->addHolder(this);
			#endif
				((Object*)object.get())->acquire();
			}
		}

		void releaseObject() {
			if (object != NULL) {
			#ifdef TRACE_REFERENCES
				object->removeHolder(this);
			#endif
				((Object*)object.get())->release();
				object = NULL;
			}
		}
	};

	

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*REFERENCE_H_*/
