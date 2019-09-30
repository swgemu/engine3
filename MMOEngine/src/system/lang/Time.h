/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "system/platform.h"

#ifndef PLATFORM_WIN
#include <unistd.h>
#include <cstdio>
#include <ctime>
#else
#include <windows.h>
#include <windef.h>
#include <pthread.h>
#endif

#include "String.h"
#include "StringBuffer.h"

#include "Long.h"

namespace sys {
  namespace lang {

	class Time {
	public:
#if defined(PLATFORM_WIN)
		typedef int ClockType;

		const static ClockType REAL_TIME = 0;
		const static ClockType THREAD_TIME = 0;
		const static ClockType PROCESS_TIME = 0;
		const static ClockType MONOTONIC_TIME = 0;
#else
		typedef clockid_t ClockType;

		const static ClockType REAL_TIME = CLOCK_REALTIME;
		const static ClockType THREAD_TIME = CLOCK_THREAD_CPUTIME_ID;
		const static ClockType PROCESS_TIME = CLOCK_PROCESS_CPUTIME_ID;
		const static ClockType MONOTONIC_TIME = CLOCK_MONOTONIC;
#endif
	private:
		struct timespec ts;

	#ifdef PLATFORM_WIN
		#define TIMESPEC_TO_FILETIME_OFFSET (((LONGLONG)27111902u << 32) + (LONGLONG)3577643008u)

		static void filetime_to_timespec(const FILETIME *ft, struct timespec *ts) {
			ts->tv_sec = (int)((*(LONGLONG *)ft - TIMESPEC_TO_FILETIME_OFFSET) / 10000000u);
			ts->tv_nsec = (int)((*(LONGLONG *)ft - TIMESPEC_TO_FILETIME_OFFSET - ((LONGLONG)ts->tv_sec * (LONGLONG)10000000u)) * 100);
		}
	#endif

	public:
		explicit Time(ClockType type = REAL_TIME) noexcept {
			updateToCurrentTime(type);
		}

		explicit Time(uint32 seconds) noexcept {
			ts.tv_sec = seconds;
			ts.tv_nsec = 0;
		}

		Time(const Time& time) = default;

		bool toString(String& str) const {
			StringBuffer msg;
			msg << ts.tv_sec << "," << ts.tv_nsec;

			str = msg.toString();
			return true;
		}

		bool parseFromString(const String& str, int version = 0) {
			int separator = str.indexOf(',');

			if (separator == -1)
				return false;

			String sec = str.subString(0, separator);
			String nsec = str.subString(separator + 1);

			ts.tv_sec = Integer::valueOf(sec);
			ts.tv_nsec = Integer::valueOf(nsec);

			return true;
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			stream->writeLong(ts.tv_sec);
			stream->writeLong(ts.tv_nsec);

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			ts.tv_sec = stream->readLong();
			ts.tv_nsec = stream->readLong();

			return true;
		}

		inline void updateToCurrentTime(ClockType type = REAL_TIME) {
			#if !defined(PLATFORM_WIN)
				clock_gettime(type, &ts);
			#else
				assert(type == 0);

				FILETIME ft;
				SYSTEMTIME st;

				GetSystemTime(&st);
				SystemTimeToFileTime(&st, &ft);

				filetime_to_timespec(&ft, &ts);
			#endif
		}

		inline void addMiliTime(uint64 mtime) {
			ts.tv_sec += (long) (mtime / 1000);
			ts.tv_nsec += (long) ((mtime % 1000) * 1000000);

			checkForOverflow();
		}

		inline void addMikroTime(uint64 utime) {
			ts.tv_sec += (long) (utime / 1000000);
			ts.tv_nsec += (long) ((utime % 1000000) * 1000);

			checkForOverflow();
		}

		inline void addNanoTime(uint64 ntime) {
			ts.tv_sec += (long) (ntime / 1000000000);
			ts.tv_nsec += (long) (ntime % 1000000000);

			checkForOverflow();
		}

		Time& operator=(const Time& t) = default;

		Time& operator=(uint32 seconds) {
			ts.tv_sec = seconds;
			ts.tv_nsec = 0;

			return *this;
		}

		int compareTo(const Time& t) const {
			if (ts.tv_sec < t.ts.tv_sec)
				return 1;
			else if (ts.tv_sec > t.ts.tv_sec)
				return -1;
			else {
				if (ts.tv_nsec < t.ts.tv_nsec)
					return 1;
				else if (ts.tv_nsec > t.ts.tv_nsec)
					return -1;
				else
					return 0;
			}

		}

		String getFormattedTime() const {
			char str[100];

			char* ret = ctime_r(&ts.tv_sec, str);

			if (ret != nullptr)
				return String(ret, strlen(str) - 1);
			else
				return String("");
		}

		String getFormattedTimeFull() const {
			int ret;
			struct tm t;
			String value;
			char buf[128];
			int len = sizeof(buf);

			if (localtime_r(&(ts.tv_sec), &t) == nullptr)
				return value;

			ret = strftime(buf, len, "%Y-%m-%dT%H:%M:%S", &t);
			if (ret <= 0)
				return value;

			len -= ret - 1;

			auto ret2 = snprintf(&buf[ret], len, ".%09ld", ts.tv_nsec);
			if (ret2 < 0 || ret2 >= len)
				return value;

			len -= ret2;

			char tz[32];

			strftime(tz, sizeof(tz), "%z", &t);

			snprintf(&buf[ret + ret2], len, "%s", tz);

			value = buf;

			return value;
		}

		int compareMiliTo(const Time& t) const {
			uint64 t1 = getMiliTime();
			uint64 t2 = t.getMiliTime();

			if (t1 < t2)
				return 1;
			else if (t1 > t2)
				return -1;
			else
				return 0;
		}

		inline static uint64 currentNanoTime(ClockType type = REAL_TIME) {
			#if !defined(PLATFORM_WIN)
				struct timespec cts;
				clock_gettime(type, &cts);

				uint64 time;

				time = cts.tv_sec;
				time = (time * 1000000000) + (uint64)cts.tv_nsec;

				return time;
			#else
				E3_ABORT("Method not supported in windows");
				return 0;
			#endif
		}

		inline bool isPast() const {
			Time t;
			return compareTo(t) > 0;
		}

		inline bool isPresent() const {
			Time t;
			return compareTo(t) == 0;
		}

		inline bool isFuture() const {
			Time t;
			return compareTo(t) < 0;
		}

	protected:
		inline void checkForOverflow() {
			if (ts.tv_nsec >= 1000000000) {
				ts.tv_sec++;
				ts.tv_nsec -= 1000000000;
			}
		}

	public:
		// getters
		inline uint32 getTime() const {
			return ts.tv_sec;
		}

		inline uint64 getMiliTime() const {
		    uint64 time;

		    time = ts.tv_sec;
		    time = (time * 1000) + (uint64)(ts.tv_nsec / 1000000.f);

		    return time;
		}

		inline uint64 getMikroTime() const {
		    uint64 time;

		    time = ts.tv_sec;
		    time = (time * 1000000) + (uint64)(ts.tv_nsec / 1000.f);

		    return time;
		}

		inline uint64 getNanoTime() const {
		    uint64 time;

		    time = ts.tv_sec;
		    time = (time * 1000000000) + (uint64)ts.tv_nsec;

		    return time;
		}

		inline int64 miliDifference(const Time& t) const {
			return t.getMiliTime() - getMiliTime();
		}

		inline int64 miliDifference(ClockType type = REAL_TIME) const {
			return Time(type).getMiliTime() - getMiliTime();
		}

		inline struct timespec* getTimeSpec() {
			return &ts;
		}

		inline const struct timespec* getTimeSpec() const {
			return &ts;
		}

		friend class AtomicTime;

	};

	class SerializableTime : public Time, public Variable {
	public:

		SerializableTime() : Time(), Variable() {
		}

		SerializableTime(const SerializableTime& time) : Time(time), Variable() {
		}

		SerializableTime& operator=(const Time& time) {
			Time::operator=(time);

			return *this;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			return Time::parseFromBinaryStream(stream);
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			return Time::toBinaryStream(stream);
		}


	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

