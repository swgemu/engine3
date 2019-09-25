/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef LOGGER_H_
#define LOGGER_H_

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
#include "system/lang/Function.h"

namespace engine {
  namespace log {

	class Logger;

	template <typename T>
	class HasToStringDataMethodSFINAE {
		typedef char success;
		struct failure { char x[2]; };

		template <class C> static success test(decltype(&C::toStringData)) ;
		template <class C> static failure test(...);

	public:
		enum { value = sizeof(test<T>(0)) == sizeof(char) };
	};

	class LoggerHelper {
	protected:
		const Logger& logger;
		const int logLevel;
		const bool boolParam;
		bool willLog;

		StringBuffer buffer;

	public:
		LoggerHelper() = delete;

		LoggerHelper(const Logger& logger, const int logLevel, const bool boolParam);
		LoggerHelper(LoggerHelper&& loggerHelper);

		~LoggerHelper();

		void flush(bool clearBuffer = true);

		template<typename T, std::enable_if_t<HasToStringDataMethodSFINAE<T>::value, int> = 0>
		LoggerHelper& operator<<(const T& a) {
			if (!willLog) {
				return *this;
			}

			buffer << a.toStringData();

			return *this;
		}

		template<typename T, std::enable_if_t<!HasToStringDataMethodSFINAE<T>::value, int> = 0>
		LoggerHelper& operator<<(const T& a) {
			if (!willLog) {
				return *this;
			}

			buffer << a;

			return *this;
		}

		StringBuffer& getBuffer() {
			return buffer;
		}

		const StringBuffer& getBuffer() const {
			return buffer;
		}

		const Logger& getLogger() const {
			return logger;
		}

		bool getWillLog() const {
			return willLog;
		}

		int getLogLevel() const {
			return logLevel;
		}

		bool getBoolParam() const {
			return boolParam;
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

		enum CallbackResult {
			SUCCESS = 0,
			DONTLOG = 1
		};

		using LoggerCallback = Function<int(LogLevel level, const char* message)>;

	private:
		FileWriter* logFile = nullptr;

		LogLevel logLevel = LOG;
		bool doGlobalLog = true;
		bool doSyncLog = true;
		bool logTimeToFile = true;
		bool logLevelToFile = true;

		String name;

		bool logJSON = false;
		bool logToConsole = true;

		UniqueReference<LoggerCallback*> callback;

		static AtomicReference<FileWriter*> globalLogFile;
		static volatile int globalLogLevel;
		static bool syncGlobalLog;
		static bool jsonGlobalLog;
		static Time starttime;

	public:
		Logger(LogLevel level = LOG);
		Logger(const String& s, LogLevel level = LOG);
		Logger(const Logger& logger);

		Logger(Logger&& logger);

		~Logger();

		Logger& operator=(const Logger& logger);
		Logger& operator=(Logger&& logger);

		static void setGlobalFileLogger(const String& file);
		static void setGlobalFileLogLevel(LogLevel level);
		static void setGlobalFileLoggerSync(bool val);
		static void setGlobalFileJson(bool val);

		static void closeGlobalFileLogger();

		void setFileLogger(const String& file, bool appendData = false);

		void closeFileLogger();

		void info(const char* msg, bool forcedlog) const;

	 	void info(const char* msg) const {
			info(msg, false);
		}

		void info(const String& msg, bool forcedLog = false) const;
		void info(const StringBuffer& msg, bool forcedLog = false) const;

		LoggerHelper info(bool forcedLog = false) const {
			return LoggerHelper(*this, LogLevel::INFO, forcedLog);
		}

		void log(const char *msg, LogLevel type, bool forceSync = false) const;

		void log(const char *msg) const {
			log(msg, LogLevel::LOG);
		}

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

		[[ noreturn ]] void fatal(const char* msg) const;
		[[ noreturn ]] void fatal(const String& msg) const;
		[[ noreturn ]] void fatal(const StringBuffer& msg) const;

		LoggerHelper fatal(bool assertion) const {
			return LoggerHelper(*this, LogLevel::FATAL, assertion);
		}

		LoggerHelper fatal() const {
			return LoggerHelper(*this, LogLevel::FATAL, false);
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

		void debug(const char* msg) const;
		void debug(const String& msg) const;
		void debug(const StringBuffer& msg) const;

		LoggerHelper debug() const {
			return LoggerHelper(*this, LogLevel::DEBUG, false);
		}

		void warning(const char* msg) const;
		void warning(const String& msg) const;
		void warning(const StringBuffer& msg) const;

		LoggerHelper warning() const {
			return LoggerHelper(*this, LogLevel::WARNING, false);
		}

		bool hasToLog(LogLevel level) const {
			return logLevel >= level || (doGlobalLog && globalLogFile && globalLogLevel >= level);
		}

		static void getTime(String& time, bool getFull = true);
		static void getTime(StringBuffer& time, bool getFull = true);

		static void getJSONString(StringBuffer& output, const char* logName, const char* msg, LogLevel type);

		static void printTime(bool getFull = true, bool err = false);

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

		inline void setLoggerCallback(LoggerCallback&& funct) {
			callback = new LoggerCallback(std::move(funct));
		}

		inline void setLoggerCallback(const LoggerCallback& funct) {
			callback = new LoggerCallback(funct);
		}

		inline void clearLoggerCallback() {
			callback = nullptr;
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

		inline LoggerCallback* getLoggerCallback() {
			return callback;
		}

		inline const LoggerCallback* getLoggerCallback() const {
			return callback;
		}

	};

  } // namespace log
} // namespace engine

using namespace engine::log;

#endif /*LOGGER_H_*/
