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

  class Object;

class StrongAndWeakReferenceCount {
protected:
	ReferenceCounter strongReferenceCount;
	ReferenceCounter weakReferenceCount;
	Object* object;
public:
	StrongAndWeakReferenceCount(uint32 strongCount, uint32 weakCount, Object* obj) :
		strongReferenceCount(strongCount), weakReferenceCount(weakCount), object(obj) {
	}

	StrongAndWeakReferenceCount(const StrongAndWeakReferenceCount& r) :
		strongReferenceCount(r.strongReferenceCount), weakReferenceCount(r.weakReferenceCount),
		object(r.object) {
	}

	StrongAndWeakReferenceCount& operator=(const StrongAndWeakReferenceCount& r) {
		if (this == &r)
			return *this;

		strongReferenceCount = r.strongReferenceCount;
		weakReferenceCount = r.weakReferenceCount;
		object = r.object;

		return *this;
	}

	inline void markAsDestroyed() volatile {
		strongReferenceCount.increaseCount();
		strongReferenceCount.setLowestBit();
	}

	inline uint32 increaseStrongCount() volatile {
		return strongReferenceCount.increaseCount();
	}

	inline uint32 increaseWeakCount() volatile {
		return weakReferenceCount.increaseCount();
	}

	inline uint32 decrementAndTestAndSetStrongCount() volatile {
		return strongReferenceCount.decrementAndTestAndSet();
	}

	inline uint32 decrementAndTestAndSetWeakCount() volatile {
		return weakReferenceCount.decrementAndTestAndSet();
	}

	inline void clearStrongCountLowestBit() volatile {
		strongReferenceCount.clearLowestBit();
	}

	inline uint32 getStrongReferenceCount() volatile {
		return strongReferenceCount.getReferenceCount();
	}

	void setObject(Object* obj) {
		object = obj;
	}

	Object* getObject() {
		return object;
	}

	template <class O>
	O getObjectReference() {
		return dynamic_cast<O>(object);
	}

};

  }
}


#endif /* STRONGANDWEAKREFERENCECOUNT_H_ */
