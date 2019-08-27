/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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

	class Logger;

	class LoggerHelper {
	protected:
		const Logger& logger;
		int logLevel;
		bool forcedLog;

		StringBuffer stream;

	public:
		LoggerHelper(const Logger& logger, int logLevel, bool forcedLog);
		~LoggerHelper();

		template<typename T>
		LoggerHelper& operator<<(const T& a) {
			stream << a;

			return *this;
		}
	};

	class Logger {
	public:
		static Logger console;

		enum LogLevel {
			NONE = -1,

			FATAL = 0,
			ERROR = 1,
			WARNING = 2,

			LOG = 3,
			INFO = 4,
			DEBUG = 5
		};

	private:
		String name;

		static AtomicReference<FileWriter*> globalLogFile;
		static volatile int globalLogLevel;
		static bool syncGlobalLog;
		static bool jsonGlobalLog;

		FileWriter* logFile;

		static Time starttime;

		LogLevel logLevel;

		bool doGlobalLog;
		bool doSyncLog;
		bool logTimeToFile;
		bool logLevelToFile;
		bool logJSON;
		bool logToConsole;

		//Mutex writeLock;

	public:
		Logger();
		Logger(const char *s);
		Logger(const String& s);

		~Logger();

		static void setGlobalFileLogger(const char* file);
		static void setGlobalFileLogger(const String& file);
		static void setGlobalFileLogLevel(LogLevel level);
		static void setGlobalFileLoggerSync(bool val);
		static void setGlobalFileJson(bool val);

		static void closeGlobalFileLogger();

		void setFileLogger(const String& file, bool appendData = false);

		void closeFileLogger();

		void info(const char *msg, bool forcedLog = false) const;
		void info(const String& msg, bool forcedLog = false) const;
		void info(const StringBuffer& msg, bool forcedLog = false) const;

		LoggerHelper info(bool forcedLog = false) const {
			return LoggerHelper(*this, LogLevel::INFO, forcedLog);
		}

		void log(const char *msg, LogLevel type = LogLevel::LOG, bool forceSync = false) const;
		void log(const String& msg) const;
		void log(const StringBuffer& msg) const;

		LoggerHelper log(bool forceSync = false) const {
			return LoggerHelper(*this, LogLevel::LOG, forceSync);
		}

		void error(const char* msg) const;
		void error(const String& msg) const;
		void error(const StringBuffer& msg) const;

		LoggerHelper error() const {
			return LoggerHelper(*this, LogLevel::ERROR, false);
		}

		void fatal(const char* msg) const;
		void fatal(const String& msg) const;
		void fatal(const StringBuffer& msg) const;

		LoggerHelper fatal(bool assertion) const {
			return LoggerHelper(*this, LogLevel::FATAL, assertion);
		}

		inline void fatal(bool assertion, const char* msg) const {
			if (!assertion) {
				fatal(msg);
			}
		}

		inline void fatal(bool assertion, const String& msg) const {
			if (!assertion) {
				fatal(msg);
			}
		}

		inline void fatal(bool assertion, const StringBuffer& msg) const {
			if (!assertion) {
				fatal(msg);
			}
		}

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

		LoggerHelper debug() const {
			return LoggerHelper(*this, LogLevel::DEBUG, false);
		}

		void warning(const char* msg) const;
		void warning(const String& msg) const;
		void warning(const StringBuffer& msg) const;

		LoggerHelper warning() const {
			return LoggerHelper(*this, LogLevel::WARNING, false);
		}

		static void getTime(String& time, bool getFull = true);
		static void getTime(StringBuffer& time, bool getFull = true);

		static void getJSONString(StringBuffer& output, const char* logName, const char* msg, LogLevel type);

		static void printTime(bool getFull = true);

		void getLogType(StringBuffer& buffer, LogLevel type) const;
		static const char* getLogType(LogLevel type);

		static uint64 getElapsedTime();

		static String escapeJSON(const String& input);
		static String unescapeJSON(const String& input);

		// setters
		inline void setLogging(bool doLog) {
			if (doLog)
				logLevel = LogLevel::DEBUG;
			else
				logLevel = LogLevel::LOG;
		}

		inline void setLogLevel(LogLevel level) {
			logLevel = level;
		}

		inline void setInfoLogLevel() {
			logLevel = LogLevel::INFO;
		}

		inline void setDebugLogLevel() {
			logLevel = LogLevel::DEBUG;
		}

		inline void setGlobalLogging(bool doLog) {
			doGlobalLog = doLog;
		}

		inline void setLogToConsole(bool doLog) {
			logToConsole = doLog;
		}

		inline void setSyncFileLogging(bool val) {
			doSyncLog = val;
		}

		inline void setLogTimeToFile(bool val) {
			logTimeToFile = val;
		}

		inline void setLogLevelToFile(bool val) {
			logLevelToFile = val;
		}

		inline void setLoggingName(const char* s) {
			name = s;
		}

		inline void setLoggingName(const String& s) {
			name = s;
		}

		inline void setLogJSON(bool val) {
			logJSON = val;
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

		inline LogLevel getLogLevel() const {
			return logLevel;
		}

		inline bool getLogJSON() const {
			return logJSON;
		}

	};

  } // namespace log
} // namespace engine

using namespace engine::log;

#endif /*LOGGER_H_*/
