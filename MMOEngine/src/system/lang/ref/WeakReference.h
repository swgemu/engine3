/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef WEAKREFERENCE_H_
#define WEAKREFERENCE_H_

#include "../Variable.h"
#include "../../thread/ReadWriteLock.h"

namespace sys {
  namespace lang {

   class WeakReferenceBase {
   public:
	   virtual ~WeakReferenceBase() {

	   }
   protected:
	   virtual void clearObject() = 0;

	   friend class Object;
   };

	template<class O> class WeakReference : public Variable, public WeakReferenceBase {
	protected:
		O object;
		ReadWriteLock rwlock;

	public:
		WeakReference() : Variable() {
			object = NULL;
		}

		WeakReference(const WeakReference<O>& ref) : Variable(), WeakReferenceBase() {
			initializeObject(ref.object);
		}

		WeakReference(O obj) : Variable() {
			initializeObject(obj);
		}

		virtual ~WeakReference() {
			releaseObject();
		}

		virtual int compareTo(const WeakReference<O>& val) const {
			if (object < val.object)
				return 1;
			else if (object > val.object)
				return -1;
			else
				return 0;
		}

		WeakReference<O>& operator=(const WeakReference<O>& ref) {
			if (this == &ref)
				return *this;

			updateObject(ref.object);

			return *this;
		}

		void operator=(O obj) {
			updateObject(obj);
		}

		O operator->() const {
			return object;
		}

		operator O() const {
			return object;
		}

		inline O get() {
			rwlock.wlock();

			O copy = object;

			if (object != NULL && object->_isGettingDestroyed()) {
				rwlock.unlock();
				return NULL;
			}

			rwlock.unlock();

			return copy;
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
		inline void updateObject(O obj) {
			if (obj == object)
				return;

			releaseObject();

			setObject(obj);
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
				object->acquireWeak(this);
			}
		}

		inline void releaseObject() {
			if (object != NULL) {
				object->releaseWeak(this);
				clearObject();
			}
		}

		void clearObject() {
			rwlock.wlock();

			if (object != NULL) {
				object = NULL;
			}

			rwlock.unlock();
		}

		friend class Object;
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /* WEAKREFERENCE_H_ */
