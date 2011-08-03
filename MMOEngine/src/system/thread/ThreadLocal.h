/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef THREADLOCAL_H_
#define THREADLOCAL_H_

#include "system/platform.h"
#include "system/lang/types.h"

#include <pthread.h>

#include <signal.h>

namespace sys {
 namespace thread {

	template<class T> class ThreadLocal {
		pthread_key_t dataKey;

	public:
		ThreadLocal();

		virtual ~ThreadLocal();

		T get();

		void remove();

		void set(const T& value);

	protected:
		void createKey();

		void deleteKey();

		virtual T initValue() {
			return TypeInfo<T>::nullValue();
		}

		T getValue();
	};

	template<class T> ThreadLocal<T>::ThreadLocal() {
		createKey();
	}

	template<class T> ThreadLocal<T>::~ThreadLocal() {
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
		set(NULL);
	}

	template <class T> void ThreadLocal<T>::set(const T& value) {
		pthread_setspecific(dataKey, (void*) value);
	}

	template<class T> void ThreadLocal<T>::createKey() {
		if (pthread_key_create(&dataKey, NULL) != 0) {
			raise(SIGSEGV);
		}
	}

	template<class T> void ThreadLocal<T>::deleteKey() {
		pthread_key_delete(dataKey);
	}

	template<class T> T ThreadLocal<T>::getValue() {
		return (T) pthread_getspecific(dataKey);
	}
 }
}

using namespace sys::thread;

#endif /* THREADLOCAL_H_ */
