/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef TIMER_H_
#define TIMER_H_

#include "system/lang/Time.h"
#include "system/lang/Function.h"
#include "system/lang/Runnable.h"

namespace sys {
	namespace util {

		class Timer {
			uint64 startTime = 0;
			uint64 elapsedTime = 0;

			uint64 totalTime = 0;

			Time::ClockType clockType = Time::MONOTONIC_TIME;

		public:
			explicit Timer(Time::ClockType type) : clockType(type) {
			}

			Timer() = default;
			Timer(const Timer& timer) = default;

			Timer& operator=(const Timer& timer) = default;

			uint64 run(const sys::lang::Function<void()>& function) {
				start();

				function();

				return stop();
			}

			uint64 run(Runnable* runnable) {
				start();

				runnable->run();

				return stop();
			}

			void start() {
				E3_ASSERT(startTime == 0);

				startTime = Time::currentNanoTime(clockType);
			}

			uint64 stop() {
				E3_ASSERT(startTime != 0);

				elapsedTime = elapsedToNow();
				startTime = 0;

				totalTime += elapsedTime;

				return elapsedTime;
			}

			void clear() {
				startTime = 0;
				elapsedTime = 0;
				totalTime = 0;
			}

			uint64 stopMs() {
				return stop() / 1000000;
			}

			uint64 elapsedToNow() const {
				if (startTime != 0)
					return Time::currentNanoTime(clockType) - startTime;
				else
					return 0;
			}

			uint64 elapsedMs() const {
				return elapsedToNow() / 1000000;
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

			uint64 getStartTimeMs() const {
				return startTime / 1000000;
			}

			uint64 getElapsedTimeMs() const {
				return elapsedTime / 1000000;
			}

			uint64 getTotalTimeMs() const {
				return totalTime / 1000000;
			}

			Time::ClockType getClockType() const {
				return clockType;
			}
		};

	} // namespace util
} // namespace sys

using namespace sys::util;


#endif /* TIMER_H_ */
