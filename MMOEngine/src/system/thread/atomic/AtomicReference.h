/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef ATOMICREFERENCE_H_
#define ATOMICREFERENCE_H_

#include <atomic>

#include "system/platform.h"

//#include "system/lang/ref/Reference.h"

namespace sys {
  namespace thread {
	namespace atomic {

	 template<class V> class AtomicReference {
		std::atomic<V> value{};

	public:
		AtomicReference() {
		}

		AtomicReference(AtomicReference&& ref) : value(ref.value.load(std::memory_order_relaxed)) {
		}

		AtomicReference(V ref) : value(ref) {
		}

		AtomicReference(const AtomicReference& r) : value(r.value.load(std::memory_order_relaxed)) {
		}

		AtomicReference& operator=(const AtomicReference& r) {
			value.store(r.value.load(std::memory_order_relaxed));

			return *this;
		}

		AtomicReference& operator=(const V val) {
			value.store(val);

			return *this;
		}

		inline V compareAndSetReturnOld(volatile V oldval, V newval) {
			V ref = oldval;
			value.compare_exchange_strong(ref, newval);

			return ref;
		}

		inline bool compareAndSet(volatile V oldval, V newval) {
			V ref = oldval;

			return value.compare_exchange_strong(ref, newval);
		}

		inline bool compareAndSetWeak(volatile V oldval, V newval) {
			V ref = oldval;

			return value.compare_exchange_weak(ref, newval);
		}

		inline V get(std::memory_order t = std::memory_order_relaxed) const {
			return value.load(t);
		}

		void set(V val, std::memory_order t = std::memory_order_seq_cst) volatile {
			value.store(val, t);
		}

		operator V() const {
			return get();
		}

		inline V operator->() const {
			return get();
		}

		inline bool operator== (const V val) const {
			return val == value.load(std::memory_order_relaxed);
		}

	};

	} // namespace atomic
  } // namespace thread
} //namespace sys

using namespace sys::thread::atomic;

#endif /*ATOMICREFERENCE_H_*/
