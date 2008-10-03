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

namespace sys {
  namespace lang {
		
	class Time {
		struct timespec ts;

	#ifdef PLATFORM_WIN
		#define TIMESPEC_TO_FILETIME_OFFSET (((LONGLONG)27111902 << 32) + (LONGLONG)3577643008)

		static void filetime_to_timespec(const FILETIME *ft, struct timespec *ts) {
			ts->tv_sec = (int)((*(LONGLONG *)ft - TIMESPEC_TO_FILETIME_OFFSET) / 10000000);
			ts->tv_nsec = (int)((*(LONGLONG *)ft - TIMESPEC_TO_FILETIME_OFFSET - ((LONGLONG)ts->tv_sec * (LONGLONG)10000000)) * 100);
		}
	#endif

	public:
		Time() {
			update();
		}
		
		inline void update() {
			#ifndef PLATFORM_WIN
				clock_gettime(CLOCK_REALTIME, &ts);
			#else
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

		Time& operator=(Time& t) {
			ts = t.ts;
			
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
		
		inline static uint64 currentNanoTime() {
			#ifndef PLATFORM_WIN
				struct timespec cts;
				clock_gettime(CLOCK_REALTIME, &cts);

				uint64 time;
		    
				time = cts.tv_sec;
				time = (time * 1000000000) + (uint64)cts.tv_nsec;
	
				return time;
			#else
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
