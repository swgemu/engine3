/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TIMER_H_
#define TIMER_H_

#include "system/lang/Time.h"

namespace sys {
  namespace util {

    class Timer {
    	uint64 startTime;
    	uint64 elapsedTime;

    	uint64 totalTime;

		Time::ClockType clockType;

    public:
    	Timer(Time::ClockType type = Time::THREAD_TIME) : startTime(0), elapsedTime(0), totalTime(0), clockType(type) {
    	}

    	void start() {
    		assert(startTime == 0);

    		startTime = Time::currentNanoTime(clockType);
    	}

    	uint64 stop() {
    		assert(startTime != 0);

    		elapsedTime = elapsed();
    		startTime = 0;

    		totalTime += elapsedTime;

    		return elapsedTime;
    	}

    	uint64 elapsed() const {
    		if (startTime != 0)
    			return Time::currentNanoTime(clockType) - startTime;
    		else
    			return 0;
    	}

    	uint64 getStartTime() const {
    		return startTime;
    	}

    	uint64 getElapsedTime() const {
    		return elapsedTime;
    	}

    	uint64 getTotalTime() const {
    		return totalTime;
    	}
    };

  } // namespace util
} // namespace sys

using namespace sys::util;


#endif /* TIMER_H_ */
