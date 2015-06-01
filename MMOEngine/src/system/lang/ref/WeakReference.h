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
			StrongAndWeakReferenceCount* p = ref.safeRead();

			initializeObject(p);

			release(p);
		}

		WeakReference(O obj) : Variable() {
			initializeObject(obj);
		}

#ifdef CXX11_COMPILER
		WeakReference(WeakReference<O>&& ref) : Variable(), weakReference(ref.weakReference) {
			ref.weakReference = NULL;
		}

		WeakReference<O>& operator=(WeakReference<O>&& ref) {
			if (this == &ref)
				return *this;

			releaseObject();

			weakReference = ref.weakReference;

			ref.weakReference = NULL;

			return *this;
		}
#endif

		virtual ~WeakReference() {
			releaseObject();
		}

		virtual int compareTo(const WeakReference<O>& val) const {
			Object* object = NULL;
			Object* valObject = NULL;

			StrongAndWeakReferenceCount* old = safeRead();

			if (old != NULL) {
				object = old->getObject();

				release(old);
			}

			StrongAndWeakReferenceCount* valOld = val.safeRead();

			if (valOld != NULL) {
				valObject = valOld->getObject();

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

			StrongAndWeakReferenceCount* p = ref.safeRead();

			newref(p);

			release(p);

			return *this;
		}

		template<class B>
		Reference<B> castTo() {
			Reference<B> stored = get().template castTo<B>();

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

		inline O getReferenceUnsafeDynamicCast() const {
			return dynamic_cast<O>(getReferenceUnsafe());
		}

		inline O getReferenceUnsafeStaticCast() const {
			return static_cast<O>(getReferenceUnsafe());
		}

		//returns an unsafe object pointer
		inline Object* getReferenceUnsafe() const {
			Object* object = NULL;

			StrongAndWeakReferenceCount* old = weakReference;

			if (old != NULL) {
				object = old->getObject();
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

			return NULL;
		}

		inline bool release(StrongAndWeakReferenceCount* old) const {
			if (old == NULL)
				return false;

			if (old->decrementAndTestAndSetWeakCount() == 0)
				return false;

			delete old;

			return true;
		}

		inline StrongAndWeakReferenceCount* newref(StrongAndWeakReferenceCount* newRef) {
			if (newRef != NULL)
				newRef->increaseWeakCount();

			for (;;) {
				StrongAndWeakReferenceCount* p = safeRead();

				if (weakReference.compareAndSet(p, newRef)) {
					bool deleted = release(p);

					if (!deleted)
						return p;
					else
						return NULL;
				} else
					release(p);
			}

			return NULL;
		}

		inline void initializeObject(O obj) {
			updateObject(obj);
		}

		inline void initializeObject(StrongAndWeakReferenceCount* count) {
			newref(count);
		}

		inline void releaseObject() {
			updateObject(NULL);
		}

	public:
		inline void updateObject(O obj) {
			StrongAndWeakReferenceCount* newRef = NULL;

			if (obj != NULL)
				newRef = obj->requestWeak();

			StrongAndWeakReferenceCount* old = newref(newRef);

			release(old);
		}

		friend class Object;
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /* WEAKREFERENCE_H_ */
