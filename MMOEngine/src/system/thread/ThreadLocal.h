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

		virtual ~ThreadLocal() {

		}

		T* get();

		void remove();

		void set(const T* value);

		virtual T* initValue() {
			return NULL;
		}
	};

	template<class T> ThreadLocal<T>::ThreadLocal() {
	#ifdef PLATFORM_UNIX
		if (pthread_key_create(&dataKey, NULL) != 0) {
			raise(SIGSEGV);
		}

	#endif

		T* value = initValue();
		set(value);
	}

	template<class T> T* ThreadLocal<T>::get() {
	#ifdef PLATFORM_UNIX
		return (T*) pthread_getspecific(dataKey);
	#else
		return NULL;
	#endif
	}

	template<class T> void ThreadLocal<T>:: remove() {
		set(NULL);
	}

	template <class T> void ThreadLocal<T>::set(const T* value) {
	#ifdef PLATFORM_UNIX
		pthread_setspecific(dataKey, value);
	#endif
	}

 }
}

using namespace sys::thread;

#endif /* THREADLOCAL_H_ */
