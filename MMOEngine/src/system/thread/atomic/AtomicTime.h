/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/**
 * @author      : theanswer (theanswer@Victors-MacBook-Pro.local)
 * @file        : AtomicTime
 * @created     : Monday Oct 15, 2018 22:10:03 CEST
 */

#ifndef ATOMICTIME_H
#define ATOMICTIME_H

#include "system/platform.h"

#ifndef PLATFORM_WIN
#include <unistd.h>
#else
#include <windows.h>
#include <windef.h>
#include <pthread.h>
#endif

#include <time.h>

#include "system/lang/String.h"
#include "system/lang/StringBuffer.h"

#include "system/lang/Long.h"

#include "system/io/StringTokenizer.h"

#include <atomic>

namespace sys {
  namespace lang {

	class AtomicTime : public Variable {
	public:
#if defined(PLATFORM_MAC) || defined(PLATFORM_WIN)
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
		std::atomic<struct timespec> ts;

	#ifdef PLATFORM_WIN
		#define TIMESPEC_TO_FILETIME_OFFSET (((LONGLONG)27111902u << 32) + (LONGLONG)3577643008u)

		static void filetime_to_timespec(const FILETIME *ft, struct timespec *ts) {
			ts->tv_sec = (int)((*(LONGLONG *)ft - TIMESPEC_TO_FILETIME_OFFSET) / 10000000u);
			ts->tv_nsec = (int)((*(LONGLONG *)ft - TIMESPEC_TO_FILETIME_OFFSET - ((LONGLONG)ts->tv_sec * (LONGLONG)10000000u)) * 100);
		}
	#endif

	public:
		explicit AtomicTime(ClockType type = REAL_TIME) {
			updateToCurrentTime(type);
		}

		explicit AtomicTime(uint32 seconds) {
			struct timespec ts;

			ts.tv_sec = seconds;
			ts.tv_nsec = 0;

			this->ts.store(ts, std::memory_order_relaxed);
		}

		AtomicTime(const AtomicTime& time) : Variable(), ts(time.ts.load(std::memory_order_relaxed)) {
		}

		AtomicTime(const Time& time) : Variable(), ts(time.ts) {
		}

		bool toString(String& str) const {
			auto ts = this->ts.load(std::memory_order_relaxed);

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

			struct timespec ts;

			ts.tv_sec = Integer::valueOf(sec);
			ts.tv_nsec = Integer::valueOf(nsec);

			this->ts.store(ts, std::memory_order_relaxed);

			return true;
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			auto ts = this->ts.load(std::memory_order_relaxed);

			stream->writeLong(ts.tv_sec);
			stream->writeLong(ts.tv_nsec);

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			struct timespec ts;

			ts.tv_sec = stream->readLong();
			ts.tv_nsec = stream->readLong();

			this->ts.store(ts, std::memory_order_relaxed);

			return true;
		}

		inline void updateToCurrentTime(ClockType type = REAL_TIME) {
			struct timespec ts;

			#ifdef PLATFORM_MAC
				//assert(type == 0);

				struct timeval tv;
				gettimeofday(&tv, nullptr);
				TIMEVAL_TO_TIMESPEC(&tv, &ts);

			#elif !defined(PLATFORM_WIN)
				clock_gettime(type, &ts);
			#else
				assert(type == 0);

				FILETIME ft;
				SYSTEMTIME st;

				GetSystemTime(&st);
				SystemTimeToFileTime(&st, &ft);

				filetime_to_timespec(&ft, &ts);
			#endif

			this->ts.store(ts, std::memory_order_relaxed);
		}

		inline void addMiliTime(uint64 mtime) {
			auto ts = this->ts.load(std::memory_order_relaxed);

			ts.tv_sec += (long) (mtime / 1000);
			ts.tv_nsec += (long) ((mtime % 1000) * 1000000);

			checkForOverflow(ts);

			this->ts.store(ts, std::memory_order_relaxed);
		}

		inline void addMikroTime(uint64 utime) {
			auto ts = this->ts.load(std::memory_order_relaxed);

			ts.tv_sec += (long) (utime / 1000000);
			ts.tv_nsec += (long) ((utime % 1000000) * 1000);

			checkForOverflow(ts);

			this->ts.store(ts, std::memory_order_relaxed);
		}

		inline void addNanoTime(uint64 ntime) {
			auto ts = this->ts.load(std::memory_order_relaxed);

			ts.tv_sec += (long) (ntime / 1000000000);
			ts.tv_nsec += (long) (ntime % 1000000000);

			checkForOverflow(ts);

			this->ts.store(ts, std::memory_order_relaxed);
		}

		AtomicTime& operator=(const AtomicTime& t) {
			if (this == &t)
				return *this;

			this->ts.store(t.ts.load(std::memory_order_relaxed), std::memory_order_relaxed);

			return *this;
		}

		AtomicTime& operator=(const Time& t) {
			this->ts.store(t.ts, std::memory_order_relaxed);

			return *this;
		}

		AtomicTime& operator=(uint32 seconds) {
			struct timespec ts;

			ts.tv_sec = seconds;
			ts.tv_nsec = 0;

			this->ts.store(ts, std::memory_order_relaxed);

			return *this;
		}

		int compareTo(const AtomicTime& t) const {
			auto ts = this->ts.load(std::memory_order_relaxed);
			auto tsr = t.ts.load(std::memory_order_relaxed);

			if (ts.tv_sec < tsr.tv_sec)
				return 1;
			else if (ts.tv_sec > tsr.tv_sec)
				return -1;
			else {
				if (ts.tv_nsec < tsr.tv_nsec)
					return 1;
				else if (ts.tv_nsec > tsr.tv_nsec)
					return -1;
				else
					return 0;
			}

		}

		int compareTo(const Time& t) const {
			auto ts = this->ts.load(std::memory_order_relaxed);
			const auto& tsr = *t.getTimeSpec();

			if (ts.tv_sec < tsr.tv_sec)
				return 1;
			else if (ts.tv_sec > tsr.tv_sec)
				return -1;
			else {
				if (ts.tv_nsec < tsr.tv_nsec)
					return 1;
				else if (ts.tv_nsec > tsr.tv_nsec)
					return -1;
				else
					return 0;
			}

		}

		String getFormattedTime() const {
			char str[100];
			auto ts = this->ts.load(std::memory_order_relaxed);
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
			auto ts = this->ts.load(std::memory_order_relaxed);

			if (localtime_r(&(ts.tv_sec), &t) == nullptr)
				return value;

			ret = strftime(buf, len, "%Y-%m-%dT%H:%M:%S", &t);
			if (ret <= 0)
				return value;

			len -= ret - 1;

			ret = snprintf(&buf[strlen(buf)], len, ".%09ld", ts.tv_nsec);
			if (ret < 0 || ret >= len)
				return value;

			len -= ret;

			char tz[32];

			strftime(tz, sizeof(tz), "%z", &t);

			snprintf(&buf[strlen(buf)], len, "%s", tz);

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
			#ifdef PLATFORM_MAC
				//assert(type == 0);
				struct timeval tv;
				gettimeofday(&tv, nullptr);

				struct timespec cts;
				TIMEVAL_TO_TIMESPEC(&tv, &cts);

				uint64 time;

				time = cts.tv_sec;
				time = (time * 1000000000) + (uint64)cts.tv_nsec;

				return time;

			#elif !defined(PLATFORM_WIN)
				struct timespec cts;
				clock_gettime(type, &cts);

				uint64 time;

				time = cts.tv_sec;
				time = (time * 1000000000) + (uint64)cts.tv_nsec;

				return time;
			#else
				assert(0 && "Method not supported in windows");
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
		inline static void checkForOverflow(struct timespec& ts) {
			if (ts.tv_nsec >= 1000000000) {
	    			ts.tv_sec++;
	    			ts.tv_nsec -= 1000000000;
	  		}
		}

	public:
		// getters
		inline uint32 getTime() const {
			return ts.load(std::memory_order_relaxed).tv_sec;
		}

		inline Time getTimeObject() const {
			Time val(0);
			val.ts = ts.load(std::memory_order_relaxed);

			return val;
		}

		inline uint64 getMiliTime() const {
		    struct timespec ts = this->ts.load(std::memory_order_relaxed);

		    uint64 time;

		    time = ts.tv_sec;
		    time = (time * 1000) + (uint64)(ts.tv_nsec / 1000000.f);

		    return time;
		}

		inline uint64 getMikroTime() const {
		    struct timespec ts = this->ts.load(std::memory_order_relaxed);

		    uint64 time;

		    time = ts.tv_sec;
		    time = (time * 1000000) + (uint64)(ts.tv_nsec / 1000.f);

		    return time;
		}

		inline uint64 getNanoTime() const {
		    struct timespec ts = this->ts.load(std::memory_order_relaxed);
		    uint64 time;

		    time = ts.tv_sec;
		    time = (time * 1000000000) + (uint64)ts.tv_nsec;

		    return time;
		}

		inline int64 miliDifference(Time& t) const {
			return t.getMiliTime() - getMiliTime();
		}

		inline int64 miliDifference() const {
			return Time().getMiliTime() - getMiliTime();
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;



#endif /* end of include guard ATOMICTIME_H */

