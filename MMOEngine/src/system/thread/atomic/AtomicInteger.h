/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef ATOMICINTEGER_H_
#define ATOMICINTEGER_H_

#include "system/platform.h"

#include "system/lang/Variable.h"

#include "system/io/ObjectInputStream.h"
#include "system/io/ObjectOutputStream.h"

#include <atomic>

namespace sys {
  namespace thread {
	namespace atomic {

	class AtomicInteger : public Variable {
		std::atomic<uint32> value{0};

	public:
		AtomicInteger() {
		}

		AtomicInteger(uint32 val) : value(val) {
		}

		AtomicInteger(const AtomicInteger& v) : value{v.value.load(std::memory_order_relaxed)} {
		}

		~AtomicInteger() {
		}

		AtomicInteger& operator=(const AtomicInteger& val) {
			value.store(val.value.load(std::memory_order_relaxed));

			return *this;
		}

		inline uint32 add(uint32 val) {
			return value += val;
		}

		inline uint32 increment() {
			return ++value;
		}

		inline uint32 postIncrement() {
			return value++;
		}

		inline uint32 decrement() {
			return --value;
		}

		inline uint32 postDecrement() {
			return value--;
		}

		inline uint32 compareAndSetReturnOld(uint32 oldval, uint32 newval) {
			uint32 val = oldval;

			value.compare_exchange_strong(val, newval);

			return val;
		}

		inline bool compareAndSet(uint32 oldval, uint32 newval) {
			uint32 val = oldval;

			return value.compare_exchange_strong(val, newval);
		}

		inline uint32 get(std::memory_order m = std::memory_order_relaxed) const {
			return value.load(m);
		}

		void set(uint32 val) {
			value = val;
		}

		uint32 operator=(const uint32 val) {
			return value = val;
		}

		inline bool operator== (const int val) const {
			return (uint32)val == value.load(std::memory_order_relaxed);
		}

		inline operator uint32() const {
			return value.load(std::memory_order_relaxed);
		}

		bool toBinaryStream(sys::io::ObjectOutputStream* stream) {
			stream->writeInt(value);

			return true;
		}

		bool parseFromBinaryStream(sys::io::ObjectInputStream* stream) {
			*this = stream->readInt();

			return true;
		}
	};

	} // namespace atomic
  } // namespace thread
} //namespace sys

using namespace sys::thread::atomic;

#endif /*ATOMICINTEGER_H_*/
