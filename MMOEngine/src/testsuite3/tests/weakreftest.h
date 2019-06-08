/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * weakreftest.h
 *
 *  Created on: 03/06/2012
 *      Author: victor
 */

#ifndef WEAKREFTEST_H_
#define WEAKREFTEST_H_

#include "engine/engine.h"

#define SECONDS_TO_RUN 200

AtomicInteger deletedObjects;

class WeakTask : public Task {
	WeakReference<Object*> object;
public:
	WeakTask(Object* obj) {
		object = obj;
	}

	void run() {
		Reference<Object*> strong = object.get();

		if (strong != nullptr) {
			//assert(!strong->_isGettingDestroyed());
			assert(strong->getReferenceCount() > 0);

			schedule(1);
			//execute();

			//printf("\robject not deleted %d", deletedObjects.increment());
		} else {
			//printf("\robjects deleted %d", deletedObjects.increment());
		}
	}
};

class StrongTask : public Task {
	Reference<Object*> object;

public:
	StrongTask(Object* obj) {
		object = obj;
	}

	void run() {
		//schedule(25);

		object = nullptr;

		//printf("creating new strong obj\n");

		Reference<Object*> strongReference = new Object();

		Reference<Task*> task = new StrongTask(strongReference);

		for (int j = 0; j < 1000; ++j) {
			Reference<Task*> weakTask = new WeakTask(strongReference);
			weakTask->schedule(1);
		}

		task->schedule(25);
	}
};

void weakreftest() {
	printf("starting weakreftest\n");

	printf("testing object deletion without storing into a ref\n");

	Object* object = new Object();
	delete object;

	printf("done\n");
	printf("creating tasks..\n");

	for (int i = 0; i < 100; ++i) {
		Reference<Object*> strongReference = new Object();

		Reference<Task*> task = new StrongTask(strongReference);
		WeakReference<Task*> weak = task.castTo<StrongTask*>().get();

		Reference<StrongTask*> castTest = task.castTo<StrongTask*>();
		WeakReference<StrongTask*> wcastTest = weak.castTo<StrongTask*>().get();


		for (int j = 0; j < 1000; ++j) {
			Reference<Task*> weakTask = new WeakTask(strongReference);
			weakTask->schedule(1);
		}

		task->schedule(25);
	}

	int secondsToRun = Math::max(10, SECONDS_TO_RUN);

	printf("done queuing tasks\n");
	printf("seconds to run %d\n", secondsToRun);

	float i = 0;

	while (i < secondsToRun * 2) {
		Thread::sleep(500);

		printf("\rsecond:%.1f", ++i / 2.f);
	}

	printf("\ndone\n");
}

#endif /* WEAKREFTEST_H_ */
