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

//#define ENABLE_THREAD_SAFE_MUTEX_WEAK

namespace sys {
  namespace lang {

	template<class O> class WeakReference : public Variable {
	protected:
		AtomicReference<StrongAndWeakReferenceCount* > weakReference;
	public:
		WeakReference() : Variable() {
			weakReference = NULL;
		}

		WeakReference(const WeakReference<O>& ref) : Variable() {
			initializeObject(ref.weakReference);
		}

		WeakReference(O obj) : Variable() {
			initializeObject(obj);
		}

		virtual ~WeakReference() {
			releaseObject();
		}

		virtual int compareTo(const WeakReference<O>& val) const {
			O object = NULL;
			O valObject = NULL;

			StrongAndWeakReferenceCount* old = safeRead();

			if (old != NULL) {
				object = old->getObjectReference<O>();

				release(old);
			}

			StrongAndWeakReferenceCount* valOld = val.safeRead();

			if (valOld != NULL) {
				valObject = valOld->getObjectReference<O>();

				release(valOld);
			}

			if (object < valObject) {
				return 1;
			} else if (object > valObject) {
				return -1;
			} else {
				return 0;
			}
		}

		WeakReference<O>& operator=(const WeakReference<O>& ref) {
			if (this == &ref)
				return *this;

			updateObject(ref.weakReference);

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

		inline bool operator==(O obj) {
			return getReferenceUnsafe() == obj;
		}

		inline bool operator!=(O obj) {
			return getReferenceUnsafe() != obj;
		}

		//returns an unsafe object pointer
		inline O getReferenceUnsafe() const {
			O object = NULL;

			StrongAndWeakReferenceCount* old = safeRead();

			if (old != NULL) {
				object = old->getObjectReference<O>();

				release(old);
			}

			return object;
		}

		inline Reference<O> get() const {
			Reference<O> objectToReturn;
			StrongAndWeakReferenceCount* currentRef = safeRead();

			if (currentRef == NULL)
				return objectToReturn;

			if (currentRef->increaseStrongCount() & 1) {
				release(currentRef);

				return objectToReturn;
			}

			objectToReturn.initializeWithoutAcquire(currentRef->getObjectReference<O>());

			release(currentRef);

			return objectToReturn;
		}

	public:

		bool toBinaryStream(ObjectOutputStream* stream) {
			return false;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			return false;
		}

	private:
		inline StrongAndWeakReferenceCount* safeRead() const {
			for (;;) {
				StrongAndWeakReferenceCount* old = weakReference;

				if (old == NULL)
					return NULL;

				old->increaseWeakCount();

				if (old == weakReference)
					return old;
				else
					release(old);
			}
		}

		inline void release(StrongAndWeakReferenceCount* old) const {
			if (old == NULL)
				return;

			if (old->decrementAndTestAndSetWeakCount() == 0)
				return;

			delete old;
		}

		inline void newref(StrongAndWeakReferenceCount* newRef) {
			if (newRef != NULL)
				newRef->increaseWeakCount();

			for (;;) {
				StrongAndWeakReferenceCount* p = safeRead();

				if (weakReference.compareAndSet(p, newRef)) {

					release(p);

					return;
				} else
					release(p);
			}
		}

	public:

		inline void updateObject(O obj) {
			StrongAndWeakReferenceCount* newRef = NULL;

			if (obj != NULL)
				newRef = obj->requestWeak();

			newref(newRef);
		}

		inline void initializeObject(O obj) {
			updateObject(obj);
		}

		inline void initializeObject(StrongAndWeakReferenceCount* count) {
			newref(count);
		}

		void releaseObject() {
			newref(NULL);
		}

		friend class Object;
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /* WEAKREFERENCE_H_ */
