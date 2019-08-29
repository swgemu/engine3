/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef THREADLOCAL_H_
#define THREADLOCAL_H_

#include "system/platform.h"
#include "system/lang/types.h"

#include <pthread.h>

#include <signal.h>

namespace sys {
 namespace thread {

	template<typename T>
	class DefaultThreadLocalDestructor {
	public:
		const static int hasDestructor = false;

		void run(void* value) {
		}
	};

	template<typename T>
	class DefaultThreadLocalDestructor<T*> {
	public:
		const static int hasDestructor = true;

		void run(void* value) {
			if (value) {
				delete reinterpret_cast<T*>(value);
			}
		}
	};

	template<class T> class ThreadLocal {
		pthread_key_t dataKey;
		void (*keyDestructor)(void*);
	public:
		ThreadLocal();

		/**
		 *  If a destructor is specified, key_delete is not called in ThreadLocal dtor
		 * @param keyDestructor
		 */
		ThreadLocal(void (*keyDestructor)(void*));

		virtual ~ThreadLocal();

		T get();

		void remove();

		void set(const T& value);

	protected:
		void createKey(void (*keyDestructor)(void*) = nullptr);

		static void defaultThreadLocalDeleteDtor(void* value) {
			DefaultThreadLocalDestructor<T> destructor;
			destructor.run(value);
		}

		virtual void deleteKey();

		virtual T initValue() {
			return TypeInfo<T>::nullValue();
		}

		T getValue();
	};

	template<class T> ThreadLocal<T>::ThreadLocal() : keyDestructor(nullptr) {
		createKey(nullptr);
	}

	template<class T> ThreadLocal<T>::ThreadLocal(void (*keyDtor)(void*) ) : keyDestructor(nullptr) {
		createKey(keyDtor);
	}

	template<class T> ThreadLocal<T>::~ThreadLocal() {
		if (!keyDestructor)
			deleteKey();
	}

	template<class T> T ThreadLocal<T>::get() {
		T value = getValue();

		if (value == TypeInfo<T>::nullValue()) {
			value = initValue();

			set(value);
		}

		return value;
	}

	template<class T> void ThreadLocal<T>::remove() {
		set(nullptr);
	}

	template <class T> void ThreadLocal<T>::set(const T& value) {
		pthread_setspecific(dataKey, reinterpret_cast<void*>(value));
	}

	template<class T> void ThreadLocal<T>::createKey(void (*dtor)(void*)) {
		if (dtor == nullptr && TypeInfo<T>::type == TypeInfoAtomicBase<T>::POINTER) {
			keyDestructor = defaultThreadLocalDeleteDtor;
		} else {
			keyDestructor = dtor;
		}

		if (pthread_key_create(&dataKey, keyDestructor) != 0) {
			raise(SIGSEGV);
		}
	}

	template<class T> void ThreadLocal<T>::deleteKey() {
		pthread_key_delete(dataKey);
	}

	template<class T> T ThreadLocal<T>::getValue() {
		return reinterpret_cast<T>(pthread_getspecific(dataKey));
	}
 }
}

using namespace sys::thread;

#endif /* THREADLOCAL_H_ */
