/*
 * StrongAndWeakReferenceCount.h
 *
 *  Created on: 06/06/2012
 *      Author: victor
 */

#ifndef STRONGANDWEAKREFERENCECOUNT_H_
#define STRONGANDWEAKREFERENCECOUNT_H_

#include "ReferenceCounter.h"

namespace sys {
  namespace lang {

class StrongAndWeakReferenceCount {
protected:
	ReferenceCounter strongReferenceCount;
	AtomicInteger weakReferenceCount;

public:
	StrongAndWeakReferenceCount(uint32 strongCount, uint32 weakCount) :
		strongReferenceCount(strongCount), weakReferenceCount(weakCount) {
	}

	StrongAndWeakReferenceCount(const StrongAndWeakReferenceCount& r) {
		strongReferenceCount = r.strongReferenceCount;
		weakReferenceCount = r.weakReferenceCount;
	}

	StrongAndWeakReferenceCount& operator=(const StrongAndWeakReferenceCount& r) {
		if (this == &r)
			return *this;

		strongReferenceCount = r.strongReferenceCount;
		weakReferenceCount = r.weakReferenceCount;

		return *this;
	}

	uint32 increaseStrongCount() {
		return strongReferenceCount.increaseCount();
	}

	uint32 decrementAndTestAndSetStrongCount() {
		return strongReferenceCount.decrementAndTestAndSet();
	}

	void clearStrongCountLowestBit() {
		strongReferenceCount.clearLowestBit();
	}

	uint32 increaseWeakCount() {
		return weakReferenceCount.increment();
	}

	uint32 decreaseWeakCount() {
		return weakReferenceCount.decrement();
	}

	uint32 getStrongReferenceCount() {
		return strongReferenceCount.getReferenceCount();
	}

	uint32 getWeakReferenceCount() {
		return weakReferenceCount.get();
	}

};

  }
}


#endif /* STRONGANDWEAKREFERENCECOUNT_H_ */
