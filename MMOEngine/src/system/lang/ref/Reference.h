/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef REFERENCE_H_
#define REFERENCE_H_

#include "system/lang/Object.h"

#include "system/lang/Long.h"

#include "system/thread/atomic/AtomicReference.h"
#include "system/thread/atomic/AtomicInteger.h"

namespace sys {
  namespace lang {

#ifdef TRACE_REFERENCES
  	  class ReferenceIdCounter {
  	  public:
  		  static AtomicInteger nextID;
  	  };
#endif


	template<class O> class Reference : public Variable {
	protected:
		AtomicReference<O> object;

#ifdef TRACE_REFERENCES
		AtomicInteger id;
#endif

	public:
		Reference() : Variable(), object() {
			//object = NULL;
#ifdef TRACE_REFERENCES
			id = ReferenceIdCounter::nextID.increment();
#endif
		}

		Reference(const Reference& ref) : Variable() {
#ifdef TRACE_REFERENCES
			id = ReferenceIdCounter::nextID.increment();
#endif
			initializeObject(ref.object.get());
		}

#ifdef CXX11_COMPILER
		Reference(Reference<O>&& ref) : Variable(), object(ref.object) {
			ref.object = NULL;

#ifdef TRACE_REFERENCES
			id = ref.id;
			ref.id = 0;
#endif
		}
#endif

		Reference(O obj) : Variable() {
#ifdef TRACE_REFERENCES
			id = ReferenceIdCounter::nextID.increment();
#endif

			initializeObject(obj);
		}

		inline virtual ~Reference() {
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

#ifdef CXX11_COMPILER
		Reference& operator=(Reference&& ref) {
			if (this == &ref)
				return *this;

			releaseObject();

			object = ref.object;

			ref.object = NULL;

#ifdef TRACE_REFERENCES
			id = ref.id;
			ref.id = 0;
#endif

			return *this;
		}
#endif

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
			O obj = object.get();

			assert(obj != NULL);
			return obj;
		}

		operator O() const {
			return object.get();
		}

		template<class B>
		Reference<B> castTo() {
			Reference<B> stored = dynamic_cast<B>(get());
			return stored;
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

		O compareAndSetReturnOld(O oldval, O newval) {
			O oldRef = object.compareAndSetReturnOld(oldval, newval);

			if (oldval == oldRef) { //success
				if (newval != NULL) {
					newval->acquire();

					#ifdef TRACE_REFERENCES
					newval->addHolder(id);
					#endif
				}

				if (oldval != NULL) {
					#ifdef TRACE_REFERENCES
					oldval->removeHolder(id);
					#endif

					oldval->release();
				}
			}

			return oldRef;
		}

		void initializeWithoutAcquire(O obj) {
			object = obj;
		}

		bool compareAndSet(O oldval, O newval) {
			bool success = object.compareAndSet(oldval, newval);

			if (success) {
				if (newval != NULL) {
					newval->acquire();

					#ifdef TRACE_REFERENCES
					newval->addHolder(id);
					#endif
				}

				if (oldval != NULL) {
					#ifdef TRACE_REFERENCES
					oldval->removeHolder(id);
					#endif

					oldval->release();
				}
			}

			return success;
		}

	protected:
		//lock free
		inline void updateObject(O obj) {
			if (obj != NULL) {
				obj->acquire();

				#ifdef TRACE_REFERENCES
				Object* castedObject = dynamic_cast<Object*>(obj);

				castedObject->addHolder(id);
				#endif
			}

			while (true) {
				O oldobj = object.get();

				if (object.compareAndSet(oldobj, obj)) {
					if (oldobj != NULL) {
						#ifdef TRACE_REFERENCES
						Object* castedObject = dynamic_cast<Object*>(oldobj);

						castedObject->removeHolder(id);
						#endif

						oldobj->release();
					}

					return;
				}

				Thread::yield();
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
				object->addHolder(id);
			#endif
				object->acquire();
			}
		}

		inline void releaseObject() {
			if (object != NULL) {
			#ifdef TRACE_REFERENCES
				object->removeHolder(id);
			#endif
				object->release();
				object = NULL;
			}
		}
	};

	

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*REFERENCE_H_*/
