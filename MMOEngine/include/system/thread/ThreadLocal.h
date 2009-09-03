/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef THREADLOCAL_H_
#define THREADLOCAL_H_

#include "Thread.h"

template<class T> class ThreadLocal {
#ifdef PLATFORM_UNIX
	pthread_key_t dataKey;
#endif

public:
	ThreadLocal();

	T* get();

	void remove();

	void set(const T* value);

protected:
	virtual T* initValue() {
		return NULL;
	}
};

#endif /* THREADLOCAL_H_ */
