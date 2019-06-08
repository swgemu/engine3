/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * threadlocal.h
 *
 *  Created on: 14/10/2012
 *      Author: victor
 */

#ifndef THREADLOCALTEST_H_
#define THREADLOCALTEST_H_

#include "engine/engine.h"

#define THREAD_LOCAL_ITERATIONS 1000000

void testThreadLocalSpeed() {
	ThreadLocal<Object*> tl;

	tl.set(new Object());

	printf("PTHREAD_KEYS_MAX: %d\n", PTHREAD_KEYS_MAX);
	printf("starting test\n");

	Time start;

	for (int i = 0; i < THREAD_LOCAL_ITERATIONS; ++i) {
		Object* obj = tl.get();

		if (obj == nullptr)
			break;
	}

	printf("finished in %lld\n", start.miliDifference());
}


#endif /* THREADLOCAL_H_ */
