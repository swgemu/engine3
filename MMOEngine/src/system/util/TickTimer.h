/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef TICKTIMER_H_
#define TICKTIMER_H_

#include <sys/times.h>

#include "system/platform.h"

namespace sys {
  namespace util {

    class TickTimer {
    	struct tms startTicks;
    	struct tms elapsedTicks;

    public:
    	TickTimer() {
    		clearStartTicks();
    		clearElapsedTicks();
    	}

    	void start() {
    		assert(startTicks.tms_utime == 0);
    		times(&startTicks);

    		clearElapsedTicks();
    	}

    	void stop() {
    		assert(startTicks.tms_utime != 0);

    		times(&elapsedTicks);
    	}

    	uint64 elapsedUserTicks() const {
    		if (startTicks.tms_utime != 0)
    			return elapsedTicks.tms_utime - startTicks.tms_utime;
    		else
    			return 0;
    	}

    	uint64 elapsedSystemTicks() const {
    		if (startTicks.tms_utime != 0)
    			return elapsedTicks.tms_stime - startTicks.tms_stime;
    		else
    			return 0;
    	}

    	void clearStartTicks() {
    		startTicks.tms_utime = 0;
    		startTicks.tms_stime = 0;
    	}

    	void clearElapsedTicks() {
    		elapsedTicks.tms_utime = 0;
    		elapsedTicks.tms_stime = 0;
    	}

    	static uint64 getTicksPerSec() {
    		return sysconf(_SC_CLK_TCK);
    	}

    };

  } // namespace util
} // namespace sys

using namespace sys::util;


#endif /* TICKTIMER_H_ */
