/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef ATOMICLONG_H_
#define ATOMICLONG_H_

#include "system/platform.h"
#include "system/io/ObjectInputStream.h"
#include "system/io/ObjectOutputStream.h"

#include <atomic>

namespace sys {
  namespace thread {
	namespace atomic {

	class AtomicLong {
		std::atomic<uint64> value{0};

	public:
		AtomicLong() : value{0} {
		}

		AtomicLong(uint64 val) : value{val} {
		}

		AtomicLong(const AtomicLong& val) : value{val.value.load(std::memory_order_relaxed)} {

		}

		AtomicLong& operator=(const AtomicLong& v) {
			value.store(v.value.load(std::memory_order_relaxed));

			return *this;
		}

		AtomicLong& operator=(const uint64 val) {
			value.store(val);

			return *this;
		}

		inline uint64 increment() {
			return ++value;
		}

		inline uint64 decrement() {
			return --value;
		}

		inline uint64 add(uint64 val) {
			return value += val;
		}

		inline bool compareAndSet(uint64 oldval, uint64 newval) {
			uint64 val = oldval;

			return value.compare_exchange_strong(val, newval);
		}

		inline uint64 get() const {
			return value.load(std::memory_order_relaxed);
		}

		inline void set(uint64 val) {
			while (!compareAndSet(value, val)) ;
		}

		operator uint64() const {
			return value.load(std::memory_order_relaxed);
		}

		inline bool operator== (const uint64 val) const {
			return val == value.load(std::memory_order_relaxed);
		}

		bool toBinaryStream(sys::io::ObjectOutputStream* stream) {
			stream->writeLong(value);

			return true;
		}

		bool parseFromBinaryStream(sys::io::ObjectInputStream* stream) {
			value = stream->readLong();

			return true;
		}
	};

	} // namespace atomic
  } // namespace thread
} //namespace sys

using namespace sys::thread::atomic;

#endif /*ATOMICLONG_H_*/
