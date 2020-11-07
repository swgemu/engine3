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
#include <ctime>
#endif

#include "String.h"
#include "StringBuffer.h"

#include "Long.h"

#include <chrono>

namespace sys {
  namespace lang {

	class Time {
	public:
#if defined(PLATFORM_WIN) || defined(PLATFORM_MAC)
		enum ClockType {
			REAL_TIME,
			THREAD_TIME,
			PROCESS_TIME,
			MONOTONIC_TIME
		};
#else
		typedef clockid_t ClockType;

		const static ClockType REAL_TIME = CLOCK_REALTIME;
		const static ClockType THREAD_TIME = CLOCK_THREAD_CPUTIME_ID;
		const static ClockType PROCESS_TIME = CLOCK_PROCESS_CPUTIME_ID;
		const static ClockType MONOTONIC_TIME = CLOCK_MONOTONIC;
#endif
	private:
		struct timespec ts;

		template<typename convert_clock_type,
			typename convert_duration_type>
			static timespec timepointToTimespec(const std::chrono::time_point<convert_clock_type, convert_duration_type>& tp) {
			using namespace std::chrono;

			auto secs = time_point_cast<seconds>(tp);
			auto ns = time_point_cast<nanoseconds>(tp) -
				time_point_cast<nanoseconds>(secs);

			struct timespec ts;

			ts.tv_sec = static_cast<decltype(ts.tv_sec)>(secs.time_since_epoch().count());
			ts.tv_nsec = static_cast<decltype(ts.tv_nsec)>(ns.count());

			return ts;
		}

		template<typename NowType>
		static auto convertTimePointToNanos(const NowType& now) {
			using namespace std::chrono;

			auto nanos = time_point_cast<nanoseconds>(now);
			auto epoch = nanos.time_since_epoch();
			auto val = duration_cast<nanoseconds>(epoch);

			return val.count();
		}

	public:
		explicit Time(ClockType type = REAL_TIME) noexcept {
			updateToCurrentTime(type);
		}

		explicit Time(uint32 seconds) noexcept {
			ts.tv_sec = seconds;
			ts.tv_nsec = 0;
		}

		Time(const Time& time) {
			ts = time.ts;
		}

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
			#if !defined(PLATFORM_WIN) && !defined(PLATFORM_MAC)
				clock_gettime(type, &ts);
			#else
				switch (type) {
				case REAL_TIME:
					this->ts = timepointToTimespec(std::chrono::system_clock::now());
					break;
				case MONOTONIC_TIME:
					this->ts = timepointToTimespec(std::chrono::steady_clock::now());
					break;
				default:
					this->ts = timepointToTimespec(std::chrono::high_resolution_clock::now());
					break;
				}
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
			if (this == &t) {
				return *this;
			}

			ts = t.ts;

			return *this;
		}

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
#ifndef PLATFORM_WIN
			char str[100];

			char* ret = ctime_r(&ts.tv_sec, str);

			if (ret != nullptr)
				return String(ret, strlen(str) - 1);
			else
				return String("");
#else
			char* ret = ctime(&ts.tv_sec);

			if (ret != nullptr)
				return String(ret, strlen(ret) - 1);
			else
				return String("");
#endif
		}

		String getFormattedTimeFull() const {
			int ret;
			struct tm t;
			String value;
			char buf[128];
			int len = sizeof(buf);

#ifndef PLATFORM_WIN
			if (localtime_r(&(ts.tv_sec), &t) == nullptr)
				return value;
#else
			auto retval = localtime(&(ts.tv_sec));
			if (retval == nullptr)
				return value;

			t = *retval;
#endif

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
			#if !defined(PLATFORM_WIN) && !defined(PLATFORM_MAC)
				struct timespec cts;
				clock_gettime(type, &cts);

				uint64 time;

				time = cts.tv_sec;
				time = (time * 1000000000) + (uint64)cts.tv_nsec;

				return time;
			#else
				switch (type) {
				case REAL_TIME:
					return convertTimePointToNanos(std::chrono::system_clock::now());
				case MONOTONIC_TIME:
					return convertTimePointToNanos(std::chrono::steady_clock::now());
				default:
					return convertTimePointToNanos(std::chrono::high_resolution_clock::now());
				}
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

		SerializableTime& operator=(const SerializableTime& time) {
			if (this == &time) {
				return *this;
			}

			Time::operator=(time);

			return *this;
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

