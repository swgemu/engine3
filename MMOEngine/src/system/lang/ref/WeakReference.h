/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef WEAKREFERENCE_H_
#define WEAKREFERENCE_H_

#include "../Variable.h"

#include "system/thread/ReadWriteLock.h"
#include "system/thread/Locker.h"
#include "system/thread/atomic/AtomicReference.h"
#include "system/lang/Object.h"
#include "StrongAndWeakReferenceCount.h"

#define ENABLE_THREAD_SAFE_MUTEX_WEAK

namespace sys {
  namespace lang {


	template<class O> class WeakReference : public Variable {
	protected:
#ifdef ENABLE_THREAD_SAFE_MUTEX_WEAK
		mutable ReadWriteLock mutex;
#endif
		StrongAndWeakReferenceCount* count;
		AtomicReference<O> object;

	public:
		WeakReference() : Variable() {
			count = NULL;
			object = NULL;
		}

		WeakReference(const WeakReference<O>& ref) : Variable() {
			initializeObject(ref.get().get());
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

		void set(StrongAndWeakReferenceCount* count) {
			this->count = count;
		}

		WeakReference<O>& operator=(const WeakReference<O>& ref) {
			if (this == &ref)
				return *this;

			updateObject(ref.object);

			return *this;
		}

		template<class B>
		Reference<B> castTo() {
			Reference<B> stored;
			Reference<O> strong = get();

			stored = dynamic_cast<B>(strong.get());
			return stored;
		}

		O operator=(O obj) {
			updateObject(obj);

			return obj;
		}

		bool operator==(O obj) {
			return object == obj;
		}

		bool operator!=(O obj) {
			return object != obj;
		}

		Reference<O> get() const {
			Reference<O> objectToReturn;

#ifdef ENABLE_THREAD_SAFE_MUTEX_WEAK
			mutex.rlock();
#endif

			if (object == NULL) {
#ifdef ENABLE_THREAD_SAFE_MUTEX_WEAK
				mutex.runlock();
#endif
				return objectToReturn;
			}

			if (count->increaseStrongCount() & 1) {
#ifdef ENABLE_THREAD_SAFE_MUTEX_WEAK
				mutex.runlock();
#endif
				return objectToReturn;
			}

			objectToReturn.initializeWithoutAcquire(object);

#ifdef ENABLE_THREAD_SAFE_MUTEX_WEAK
			mutex.runlock();
#endif

			return objectToReturn;
		}

	public:

		bool toBinaryStream(ObjectOutputStream* stream) {
			return false;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			return false;
		}

		inline void updateObject(O obj) {
#ifdef ENABLE_THREAD_SAFE_MUTEX_WEAK
			Locker locker(&mutex);
#endif

			if (obj == object.get())
				return;

			if (object != NULL) {
				if (count->decreaseWeakCount() == 0) {
					delete count;
					count = NULL;
				}
			}

			object = obj;

			acquireObject();
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
				(static_cast<Object*>(object.get()))->acquireWeak(this);
			} else {
				count = NULL;
			}
		}

		void releaseObject() {
#ifdef ENABLE_THREAD_SAFE_MUTEX_WEAK
			Locker locker(&mutex);
#endif
			if (object != NULL) {
				if (count->decreaseWeakCount() == 0) {
					delete count;
					count = NULL;
				}
			}

			object = NULL;
		}

		friend class Object;
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /* WEAKREFERENCE_H_ */
