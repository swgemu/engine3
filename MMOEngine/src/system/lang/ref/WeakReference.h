/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef WEAKREFERENCE_H_
#define WEAKREFERENCE_H_

#include <functional>

#include "system/lang/Variable.h"

#include "system/thread/ReadWriteLock.h"
#include "system/thread/Locker.h"
#include "system/thread/atomic/AtomicReference.h"
#include "system/lang/Object.h"
#include "StrongAndWeakReferenceCount.h"

//#define ENABLE_THREAD_SAFE_MUTEX_WEAK

namespace sys {
  namespace lang {
	template<class T> class Reference;

	template<class O> class WeakReference {
	protected:
		AtomicReference<StrongAndWeakReferenceCount* > weakReference;
	public:
		WeakReference() {
			weakReference = nullptr;
		}

		WeakReference(const WeakReference& ref) {
			StrongAndWeakReferenceCount* p = ref.safeRead();

			initializeObject(p);

			release(p);
		}

		WeakReference(StrongAndWeakReferenceCount* p) {
			initializeObject(p);
		}

		WeakReference(O obj) {
			initializeObject(obj);
		}

#ifdef CXX11_COMPILER
		WeakReference(WeakReference<O>&& ref) : weakReference(ref.weakReference) {
			ref.weakReference = nullptr;
		}

		WeakReference<O>& operator=(WeakReference<O>&& ref) {
			if (this == &ref)
				return *this;

			releaseObject();

			weakReference = ref.weakReference;

			ref.weakReference = nullptr;

			return *this;
		}
#endif

		~WeakReference() {
			releaseObject();
		}

		int compareTo(const WeakReference<O>& val) const {
			Object* object = nullptr;
			Object* valObject = nullptr;

			StrongAndWeakReferenceCount* old = safeRead();

			if (old != nullptr) {
				object = old->getObject();

				release(old);
			}

			StrongAndWeakReferenceCount* valOld = val.safeRead();

			if (valOld != nullptr) {
				valObject = valOld->getObject();

				release(valOld);
			}

			if (std::less<Object*>()(object, valObject)) {
				return 1;
			} else if (object == valObject) {
				return 0;
			} else {
				return -1;
			}
		}

		WeakReference<O>& operator=(const WeakReference<O>& ref) {
			if (this == &ref)
				return *this;

			StrongAndWeakReferenceCount* p = ref.safeRead();

			updateObject(p);

			release(p);

			return *this;
		}

		template<class B>
		Reference<B> castTo() {
			Reference<B> stored = get().template castTo<B>();

			return stored;
		}

		template<class B>
		WeakReference<B> staticCastToWeak() {
			StrongAndWeakReferenceCount* p = safeRead();

			WeakReference<B> ref(p);

			release(p);

			return ref;
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
			Object* object = nullptr;

			StrongAndWeakReferenceCount* old = weakReference;

			if (old != nullptr) {
				object = old->getObject();
			}

			return object;
		}

		inline Reference<O> get() const {
			Reference<O> objectToReturn;
			StrongAndWeakReferenceCount* currentRef = safeRead();

			if (currentRef == nullptr)
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

	protected:
		inline StrongAndWeakReferenceCount* safeRead() const {
			for (;;) {
				StrongAndWeakReferenceCount* old = weakReference.get(std::memory_order_seq_cst);

				if (old == nullptr)
					return nullptr;

				old->increaseWeakCount();

				if (old == weakReference.get(std::memory_order_seq_cst))
					return old;
				else
					release(old);
			}

			return nullptr;
		}

		inline bool release(StrongAndWeakReferenceCount* old) const {
			if (old == nullptr)
				return false;

			if (old->decrementAndTestAndSetWeakCount() == 0)
				return false;

			delete old;

			return true;
		}

		inline StrongAndWeakReferenceCount* newref(StrongAndWeakReferenceCount* newRef) {
			if (newRef != nullptr)
				newRef->increaseWeakCount();

			for (;;) {
				StrongAndWeakReferenceCount* p = safeRead();

				if (weakReference.compareAndSetWeak(p, newRef)) {
					bool deleted = release(p);

					if (!deleted)
						return p;
					else
						return nullptr;
				} else
					release(p);
			}

			return nullptr;
		}

		inline void initializeObject(O obj) {
			updateObject(obj);
		}

		inline void initializeObject(StrongAndWeakReferenceCount* count) {
			newref(count);
		}

		inline void releaseObject() {
			updateObject((O) nullptr);
		}

	public:
		inline void updateObject(O obj) {
			StrongAndWeakReferenceCount* newRef = nullptr;

			if (obj != nullptr)
				newRef = obj->requestWeak();

			StrongAndWeakReferenceCount* old = newref(newRef);

			release(old);
		}

		inline void updateObject(StrongAndWeakReferenceCount* newRef) {
			StrongAndWeakReferenceCount* old = newref(newRef);

			release(old);
		}

		friend class Object;
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /* WEAKREFERENCE_H_ */
