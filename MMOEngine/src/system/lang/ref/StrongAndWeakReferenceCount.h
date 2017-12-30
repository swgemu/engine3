/*
 * StrongAndWeakReferenceCount.h
 *
 *  Created on: 06/06/2012
 *      Author: victor
 */

#ifndef STRONGANDWEAKREFERENCECOUNT_H_
#define STRONGANDWEAKREFERENCECOUNT_H_

#ifdef CXX11_COMPILER
#include <type_traits>
#endif

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
		uint32 ret = strongReferenceCount.decrementAndTestAndSet();

		if (ret != 0) {
			object = nullptr;
		}

		return ret;
	}

	inline bool tryStrongFinalDecrement() volatile {
		bool ret = strongReferenceCount.tryFinalDecrement();

		if (ret) {
			object = nullptr;
		}

		return ret;
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


	template<class R, bool virt>
	class Helper {
	public:
		R static convert(Object* o) {
			return R();
		}
	};

	template<class R>
	class Helper<R, false> {
	public:
		R static convert(Object* o) {
			return static_cast<R>(o);
		}
	};

	template<class R>
	class Helper<R, true> {
	public:
		R static convert(Object* o) {
			return dynamic_cast<R>(o);
		}
	};

	template <class O>
	O getObjectReference() {
#ifdef CXX11_COMPILER
		return Helper<O, std::remove_pointer<O>::type::is_virtual_object>::convert(object);
#else
		return dynamic_cast<O>(object);
#endif
	}

};

  }
}


#endif /* STRONGANDWEAKREFERENCECOUNT_H_ */
