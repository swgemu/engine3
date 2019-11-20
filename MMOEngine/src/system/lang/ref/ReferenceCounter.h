/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef REFERENCECOUNTER_H_
#define REFERENCECOUNTER_H_

#include <signal.h>

#include "system/platform.h"

#include "system/lang/System.h"

#include <atomic>

namespace sys {
  namespace lang {

	class ReferenceCounter {
	protected:
		std::atomic<uint32> _references{0};

	public:
		ReferenceCounter() {
		}

		ReferenceCounter(uint32 count) : _references(count) {
		}

		ReferenceCounter(const ReferenceCounter& counter) : _references(counter._references.load(std::memory_order_relaxed)) {
		}

		ReferenceCounter& operator=(const ReferenceCounter& counter) {
			_references.store(counter._references.load(std::memory_order_relaxed));

			return *this;
		}

	public:
		inline uint32 increaseCount() volatile {
			return _references.fetch_add(2, std::memory_order_acq_rel) + 2;
		}

		void setLowestBit() volatile {
			uint32 oldVal, newVal;

			do {
				oldVal = _references.load(std::memory_order_acquire);

				newVal = oldVal | 1;
			} while (!_references.compare_exchange_weak(oldVal, newVal,
						std::memory_order_release, std::memory_order_relaxed));
		}

		void clearLowestBit() volatile {
			uint32 oldVal, newVal;

			do {
				oldVal = _references.load(std::memory_order_acquire);

				newVal = oldVal & uint64(0xFFFFFFFFFFFFFFFE);
			} while (!_references.compare_exchange_weak(oldVal, newVal,
						std::memory_order_release, std::memory_order_relaxed));
		}

		inline bool tryFinalDecrement() volatile {
			uint32 oldVal, newVal;

			oldVal = _references.load(std::memory_order_acquire);

			if (oldVal != 2)
				return false;

			newVal = 1;

			return _references.compare_exchange_weak(oldVal, newVal,
					std::memory_order_release, std::memory_order_relaxed);
		}

		inline uint32 decrementAndTestAndSet() volatile {
			uint32 oldVal, newVal;

			do {
				oldVal = _references.load(std::memory_order_acquire);

				newVal = oldVal - 2;

				if (newVal == 0)
					newVal = 1;
			} while (!_references.compare_exchange_weak(oldVal, newVal,
						std::memory_order_release, std::memory_order_relaxed));

			return ((oldVal - newVal) & 1);
		}

		inline uint32 getReferenceCount(std::memory_order o = std::memory_order_consume) volatile const {
			return _references.load(o);
		}

		inline void reset() volatile {
			_references.store(0);
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*REFERENCECOUNTER_H_*/
