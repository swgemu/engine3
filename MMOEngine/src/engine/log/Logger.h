/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LOGGER_H_
#define LOGGER_H_

#ifdef NDEBUG
#define DISABLE_DEBUG_LOG
#endif

#ifdef NDEBUG
#define E3_INFO(x)
#define E3_DEBUG(x)
#define E3_WARNING(x)
#define E3_ERROR(x) error(x)
#define E3_LOG(x) log(x)
#define E3_FATAL(x) fatal(x)
#else
#define E3_INFO(x) info(x)
#define E3_DEBUG(x) debug(x)
#define E3_WARNING(x) warning(x)
#define E3_ERROR(x) error(x)
#define E3_LOG(x) log(x)
#define E3_FATAL(x) fatal(x)
#endif

#include "system/lang.h"

namespace engine {
  namespace log {

	class Logger {
		String name;

		static AtomicReference<FileWriter*> globalLogFile;

		FileWriter* logFile;

		static Time starttime;

		int logLevel;

		bool doGlobalLog;

		//Mutex writeLock;

	public:
		static Logger console;

		static const  int LOG = 0;
		static const int INFO = 1;
		static const int DEBUG = 2;

	public:
		Logger();
		Logger(const char *s);
		Logger(const String& s);

		~Logger();

		static void setGlobalFileLogger(const char* file);
		static void setGlobalFileLogger(const String& file);

		static void closeGlobalFileLogger();

		void setFileLogger(const String& file, bool appendData = false);

		void closeFileLogger();

		void info(const char *msg, bool forcedLog = false) const;
		void info(const String& msg, bool forcedLog = false) const;
		void info(const StringBuffer& msg, bool forcedLog = false) const;

		void log(const char *msg) const;
		void log(const String& msg) const;
		void log(const StringBuffer& msg) const;

		void error(const char* msg) const;
		void error(const String& msg) const;
		void error(const StringBuffer& msg) const;

		void fatal(const char* msg) const;
		void fatal(const String& msg) const;
		void fatal(const StringBuffer& msg) const;

#ifdef DISABLE_DEBUG_LOG
		void debug(const char* msg) const {
		}

		void debug(const String& msg) const {
		}

		void debug(const StringBuffer& msg) const {
		}
#else
		void debug(const char* msg) const;
		void debug(const String& msg) const;
		void debug(const StringBuffer& msg) const;
#endif

		void warning(const char* msg) const;
		void warning(const String& msg) const;
		void warning(const StringBuffer& msg) const;

		static void getTime(String& time, bool getFull = true);
		static void printTime(bool getFull = true);

		static uint64 getElapsedTime();

		// setters
		inline void setLogging(bool doLog) {
			if (doLog)
				logLevel = DEBUG;
			else
				logLevel = LOG;
		}

		inline void setLogLevel(int level) {
			logLevel = level;
		}

		inline void setInfoLogLevel() {
			logLevel = INFO;
		}

		inline void setDebugLogLevel() {
			logLevel = DEBUG;
		}

		inline void setGlobalLogging(bool doLog) {
			doGlobalLog = doLog;
		}

		inline void setLoggingName(const char* s) {
			name = s;
		}

		inline void setLoggingName(const String& s) {
			name = s;
		}

		// getters
		inline String& getLoggingName() {
			return name;
		}

		inline const String& getLoggingName() const {
			return name;
		}

		inline FileWriter* getFileLogger() const {
			return logFile;
		}

		inline int getLogLevel() const {
			return logLevel;
		}

	};

  } // namespace log
} // namespace engine

using namespace engine::log;

#endif /*LOGGER_H_*/
