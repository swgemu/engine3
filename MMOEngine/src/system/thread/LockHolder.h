/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "system/platform.h"
#include "system/thread/atomic/AtomicInteger.h"

namespace sys {
namespace thread {
	class LockHolder {
		static AtomicInteger lockSequence;
		const static uint8 STATE_ACTIVE = 0b00000001;
		const static uint8 STATE_RLOCK = 0b00000010;
		const static uint8 STATE_WLOCK  = 0b00000100;

		const char* mFile = nullptr;
		const char* mFunction = nullptr;
		uint32 mLockSequence;

		int16 mThreadNumber = -1;
		int16 mLine = 0;
		uint8 mState = 0;

		public:
		LockHolder() {
			mFile = nullptr;
			mFunction = nullptr;
			mThreadNumber = -1;
			mLine = 0;
			mState = 0;
		}

		inline void set(const char* modifier, int16 threadNumber, const char *file, const char *function, int line) {
			mFile = file;
			mFunction = function;
			mLockSequence = lockSequence.increment();
			mThreadNumber = threadNumber;
			mLine = line;
			mState = modifier[0] != 'r' ? STATE_WLOCK : STATE_RLOCK;
			mState |= STATE_ACTIVE;
		}

		inline void clear() {
			mState &= ~STATE_ACTIVE;
		}

		inline bool isSet() const {
			return mState & STATE_ACTIVE;
		}

		inline bool wasSet() const {
			return mState != 0;
		}

		inline bool isAvailable() const {
			return mState == 0 || !(mState & STATE_ACTIVE);
		}

		inline bool isWrite() const {
			return (mState & STATE_WLOCK) != 0;
		}


		inline uint32 getLockSequence() const {
			return mLockSequence;
		}


		inline int16 getThreadNumber() const {
			return mThreadNumber;
		}

		inline char getType() const {
			return isWrite() ? 'w' : 'r';
		}

		inline const char* getFile() const {
			return mFile != nullptr ? mFile : "unknown";
		}

		inline const char* getFunction() const {
			return mFunction != nullptr ? mFunction : "unknown";
		}

		inline int16 getLine() const {
			return mLine;
		}

		inline bool isWriteModifier(const char* modifier) const {
			return modifier[0] != 'r';
		}

		const String format() const {
			return format(getType(), getThreadNumber(), getFile(), getFunction(), getLine(), getLockSequence());
		}

		int compareTo(const LockHolder& b) const {
			if (mLockSequence == b.mLockSequence) {
				return 0;
			} else if (mLockSequence < b.mLockSequence) {
				return -1;
			}

			return 1;
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			return false;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			return false;
		}

		static const String format(char type, int16 threadNumber, const char* file, const char* function, int line, uint32 time = 0);

		static uint32 getTotalLocks() {
			return lockSequence.get();
		}
	};
}
}
