/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "system/platform.h"

typedef time_t utime;

class MTRand;

#include "system/io/PrintStream.h"

#include "system/thread/ThreadLocal.h"

#include "mersenne/MersenneTwister.h"

namespace sys {
  namespace lang {

	#ifndef CLK_TCK // Fix for undefined CLK_TCK under linux.
		#define CLK_TCK CLOCKS_PER_SEC
	#endif // CLK_TCK

	class System {
		static ThreadLocal<MTRand*> mtrand;

	public:
		static class PrintStream out;

		static inline time_t getTime() {
			return (utime) time(0);
		}

	#ifndef PLATFORM_WIN
		static inline uint64 getMiliTime() {
		    uint64 time_in_ms;

		    struct timeval tm;
		    gettimeofday(&tm, nullptr);

		    time_in_ms = tm.tv_sec; // Avoid overflow by doing mul in 64 bit int
		    time_in_ms = (time_in_ms * 1000) + (uint64) (tm.tv_usec / 1000.f);

		    return (uint64)time_in_ms;
		}

		static inline uint64 getMikroTime() {
		    uint64 time_in_ms;

		    struct timeval tm;
		    gettimeofday(&tm, nullptr);

		    time_in_ms = tm.tv_sec; // Avoid overflow by doing mul in 64 bit int
		    time_in_ms = (time_in_ms * 1000000) + tm.tv_usec;

		    return (uint64)time_in_ms;
		}
	#else
		static inline uint64 getMiliTime() {
			return uint64(((float) clock() / (float) CLOCKS_PER_SEC) * 1000.f);
		}

		static inline uint64 getMikroTime() {
			return uint64(((double) clock() / (double) CLOCKS_PER_SEC) * 1000000.f);
		}
	#endif

		static uint32 random(unsigned int bucket = 0xFFFFFFFF);

		static MTRand* getMTRand() {
			MTRand* localMT = mtrand.get();

			if (localMT == nullptr) {
				localMT = new MTRand();

				mtrand.set(localMT);
			}

			return localMT;
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*SYSTEM_H_*/
