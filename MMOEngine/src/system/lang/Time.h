/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TIME_H_
#define TIME_H_

#include "../platform.h"

#ifndef PLATFORM_WIN
#include <unistd.h>
#else
#include <windows.h>
#include <windef.h>
#include <pthread.h>
#endif

#include <time.h>

#include "String.h"
#include "StringBuffer.h"

#include "Long.h"

#include "system/io/StringTokenizer.h"

namespace sys {
  namespace lang {

	class Time : public Variable {
	public:
#if defined(PLATFORM_MAC) || defined(PLATFORM_WIN)
		typedef int ClockType;

		const static ClockType REAL_TIME = 0;
		const static ClockType THREAD_TIME = 0;
		const static ClockType PROCESS_TIME = 0;
#else
		typedef clockid_t ClockType;

		const static ClockType REAL_TIME = CLOCK_REALTIME;
		const static ClockType THREAD_TIME = CLOCK_THREAD_CPUTIME_ID;
		const static ClockType PROCESS_TIME = CLOCK_PROCESS_CPUTIME_ID;
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
		Time(ClockType type = REAL_TIME) {
			updateToCurrentTime(type);
		}

		Time(uint32 seconds) {
			ts.tv_sec = seconds;
			ts.tv_nsec = 0;
		}

		Time(const Time& time) : Variable() {
			//ts = time.ts;
			memcpy(&ts, &time.ts, sizeof(timespec));
		}

		bool toString(String& str) {
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
			#ifdef PLATFORM_MAC
				//assert(type == 0);

				struct timeval tv;
				gettimeofday(&tv, NULL);
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

		Time& operator=(const Time& t) {
			if (this == &t)
				return *this;

			//ts = t.ts;
			memcpy(&ts, &t.ts, sizeof(timespec));

			return *this;
		}

		int compareTo(Time& t) {
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

		String getFormattedTime() {
			char str[100];

			char* ret = ctime_r(&ts.tv_sec, str);

			if (ret != NULL)
				return String(ret, strlen(str) - 1);
			else
				return String("");
		}

		int compareMiliTo(Time& t) {
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
				gettimeofday(&tv, NULL);

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

		inline bool isPast() {
			Time t;
			return compareTo(t) > 0;
		}

		inline bool isPresent() {
			Time t;
			return compareTo(t) == 0;
		}

		inline bool isFuture() {
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
		inline uint32 getTime() {
			return ts.tv_sec;
		}

		inline uint64 getMiliTime() {
		    uint64 time;

		    time = ts.tv_sec;
		    time = (time * 1000) + (uint64)(ts.tv_nsec / 1000000.f);

		    return time;
		}

		inline uint64 getMikroTime() {
		    uint64 time;

		    time = ts.tv_sec;
		    time = (time * 1000000) + (uint64)(ts.tv_nsec / 1000.f);

		    return time;
		}

		inline uint64 getNanoTime() {
		    uint64 time;

		    time = ts.tv_sec;
		    time = (time * 1000000000) + (uint64)ts.tv_nsec;

		    return time;
		}

		inline int64 miliDifference(Time& t) {
			return t.getMiliTime() - getMiliTime();
		}

		inline int64 miliDifference() {
			return Time().getMiliTime() - getMiliTime();
		}

		inline struct timespec* getTimeSpec() {
			return &ts;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*TIME_H_*/
