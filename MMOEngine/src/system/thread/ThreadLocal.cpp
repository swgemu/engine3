/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ThreadLocal.h"

template<class T> ThreadLocal<T>::ThreadLocal() {
#ifdef PLATFORM_UNIX
	pthread_key_create(&dataKey, NULL);
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
	pthread_setspecific(threadDataKey, &value);
#endif
}
