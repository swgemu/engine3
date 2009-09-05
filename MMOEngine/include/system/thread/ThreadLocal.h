/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef THREADLOCAL_H_
#define THREADLOCAL_H_

#include "Thread.h"

namespace sys {
 namespace thread {

	template<class T> class ThreadLocal {
	#ifdef PLATFORM_UNIX
		pthread_key_t dataKey;
	#endif

	public:
		ThreadLocal();

		virtual ~ThreadLocal();

		T* get();

		void remove();

		void set(const T* value);

	protected:
		void createKey();

		void deleteKey();

		virtual T* initValue() {
			return NULL;
		}

		T* getValue();
	};

	template<class T> ThreadLocal<T>::ThreadLocal() {
		createKey();
	}

	template<class T> ThreadLocal<T>::~ThreadLocal() {
		deleteKey();
	}

	template<class T> T* ThreadLocal<T>::get() {
		T* value = getValue();

		if (value == NULL) {
			value = initValue();

			set(value);
		}

		return value;
	}

	template<class T> void ThreadLocal<T>:: remove() {
		set(NULL);
	}

	template <class T> void ThreadLocal<T>::set(const T* value) {
	#ifdef PLATFORM_UNIX
		pthread_setspecific(dataKey, value);
	#endif
	}

	template<class T> void ThreadLocal<T>::createKey() {
	#ifdef PLATFORM_UNIX
		if (pthread_key_create(&dataKey, NULL) != 0) {
			raise(SIGSEGV);
		}
	#endif
	}

	template<class T> void ThreadLocal<T>::deleteKey() {
	#ifdef PLATFORM_UNIX
		pthread_key_delete(dataKey);
	#endif
	}

	template<class T> T* ThreadLocal<T>::getValue() {
	#ifdef PLATFORM_UNIX
		return (T*) pthread_getspecific(dataKey);
	#else
		return NULL;
	#endif
	}
 }
}

using namespace sys::thread;

#endif /* THREADLOCAL_H_ */
