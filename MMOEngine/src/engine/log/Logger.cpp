/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "Logger.h"

AtomicReference<FileWriter*> Logger::globalLogFile = nullptr;

Time Logger::starttime;

Logger Logger::console = []() {
		Logger log("Console");
		log.setLogToConsole(true);

		return log;
	}();

volatile int Logger::globalLogLevel = LogLevel::DEBUG;
bool Logger::syncGlobalLog = false;
bool Logger::jsonGlobalLog = false;

Logger::Logger(LogLevel level) : logLevel(level) {
}

Logger::Logger(const String& s, LogLevel level) : logLevel(level), name(s) {
}

Logger::Logger(Logger&& logger) : logFile(logger.logFile), logLevel(logger.logLevel),
	doGlobalLog(logger.doGlobalLog), doSyncLog(logger.doSyncLog), logTimeToFile(logger.logTimeToFile),
	logLevelToFile(logger.logLevelToFile), name(std::move(logger.name)), logJSON(logger.logJSON),
	logToConsole(logger.logToConsole), callback(std::move(logger.callback)) {

	logger.logFile = nullptr;
}

Logger::Logger(const Logger& logger) : logFile(nullptr), logLevel(logger.logLevel),
	doGlobalLog(logger.doGlobalLog), doSyncLog(logger.doSyncLog), logTimeToFile(logger.logTimeToFile),
	logLevelToFile(logger.logLevelToFile), name(logger.name), logJSON(logger.logJSON),
	logToConsole(logger.logToConsole) {

	if (logger.callback != nullptr) {
		callback = new LoggerCallback(*logger.callback.get());
	}

	if (logger.logFile != nullptr) {
		setFileLogger(logger.logFile->getFile()->getName());
	}
}

Logger& Logger::operator=(const Logger& logger) {
	if (&logger == this)
		return *this;

	closeFileLogger();

	logFile = logger.logFile;
       	logLevel = logger.logLevel;
	doGlobalLog = logger.doGlobalLog;
       	doSyncLog = logger.doSyncLog;
       	logTimeToFile = logger.logTimeToFile;
	logLevelToFile = logger.logLevelToFile;
       	name = logger.name;
	logJSON = logger.logJSON;
	logToConsole = logger.logToConsole;

	if (logger.callback != nullptr) {
	       	callback = new LoggerCallback(*logger.callback.get());
	} else {
		callback = nullptr;
	}

	if (logger.logFile != nullptr) {
		setFileLogger(logger.logFile->getFile()->getName());
	}

	return *this;
}

Logger& Logger::operator=(Logger&& logger) {
	if (&logger == this)
		return *this;

	closeFileLogger();

	logFile = logger.logFile;
       	logLevel = logger.logLevel;
	doGlobalLog = logger.doGlobalLog;
       	doSyncLog = logger.doSyncLog;
       	logTimeToFile = logger.logTimeToFile;
	logLevelToFile = logger.logLevelToFile;
       	name = std::move(logger.name);
	logJSON = logger.logJSON;
	logToConsole = logger.logToConsole;
       	callback = std::move(logger.callback);

	logger.logFile = nullptr;

	return *this;
}

Logger::~Logger() {
	closeFileLogger();
}

void Logger::setGlobalFileLogger(const String& file) {
	if (globalLogFile != nullptr)
		closeGlobalFileLogger();

	globalLogFile = new FileWriter(new File(file), true);

	starttime.updateToCurrentTime();
}

void Logger::setGlobalFileLogLevel(LogLevel level) {
	globalLogLevel = level;
}

void Logger::setGlobalFileLoggerSync(bool val) {
	syncGlobalLog = val;
}

void Logger::setGlobalFileJson(bool val) {
	jsonGlobalLog = val;
}

void Logger::setFileLogger(const String& file, bool appendData) {
	if (logFile != nullptr)
		closeFileLogger();

	File* fileObject = new File(file);

	logFile = new FileWriter(fileObject, appendData);
}

void Logger::closeGlobalFileLogger() {
	auto globalLogFile = Logger::globalLogFile.get();

	bool success = Logger::globalLogFile.compareAndSet(globalLogFile, nullptr);

	if (success && globalLogFile != nullptr) {
		globalLogFile->close();

		delete globalLogFile->getFile();
		delete globalLogFile;
	}
}

void Logger::closeFileLogger() {
	if (logFile != nullptr) {
		logFile->close();

		delete logFile->getFile();

		delete logFile;
		logFile = nullptr;
	}
}

void Logger::info(const char *msg, bool forcedLog) const {
	if ((logToConsole && logLevel >= INFO) || forcedLog) {
		printTime(false);

		System::out << " [" << name << "] " << msg << endl;
	}

	log(msg, LogLevel::INFO);
}

void Logger::info(const String& msg, bool forcedLog) const {
	info(msg.toCharArray(), forcedLog);
}

void Logger::info(const StringBuffer& msg, bool forcedLog) const {
	info(msg.toString(), forcedLog);
}

void Logger::getJSONString(StringBuffer& output, const char* logName, const char* msg, LogLevel type) {
	Time time;
	Thread* currentThread = Thread::getCurrentThread();

	/*using json = nlohmann::json;
	json rootObject;

	rootObject["currentTimestamp"] = time.getFormattedTimeFull().toCharArray();
	rootObject["timeSinceStart"] = Logger::starttime.miliDifference(time);
	rootObject["logType"] = getLogType(type);
	rootObject["message"] = msg;
	rootObject["logName"] = logName;


	if (currentThread != nullptr)
		rootObject["thread"]  = currentThread->getName().toCharArray();*/

	output << "{\"@timestamp\":\"" << time.getFormattedTimeFull() << "\","
			<< "\"time_msec\":" << time.getMiliTime() << ","
			<< "\"uptime_secs\":" <<  Logger::starttime.miliDifference(time) / 1000 << ","
			<< "\"log_level\":\"" << getLogType(type) << "\","
			<< "\"log\":\"" << escapeJSON(msg) << "\","
			<< "\"log_tag\":\"" << logName << "\"";

	if (currentThread != nullptr) {
		output << ",\"thread\":\"" << currentThread->getName() << "\"";
	}

	output << "}";
}

void Logger::log(const char *msg, LogLevel type, bool forceSync) const {
	if (callback && (*callback.get())(type, msg)) {
		return;
	}

	if (logFile == nullptr && globalLogFile == nullptr)
		return;

	if (logLevel >= type && logFile != nullptr) {
		FileWriter* logFile = const_cast<FileWriter*>(this->logFile);

		StringBuffer fullMessage;

		if (!logJSON) {
			if (logTimeToFile) {
				getTime(fullMessage);
			}

			if (logLevelToFile) {
				getLogType(fullMessage, type);
			}

			fullMessage << msg;
		} else {
			getJSONString(fullMessage, name.toCharArray(), msg, type);
		}

		fullMessage << endl;

		(*logFile) << fullMessage;

		if (doSyncLog || forceSync) {
			logFile->flush();
		}
	} else if (doGlobalLog && globalLogFile != nullptr && globalLogLevel >= type) {
		FileWriter* globalLogFile = const_cast<FileWriter*>(this->globalLogFile.get());

		StringBuffer fullMessage;

		if (!jsonGlobalLog) {
			getTime(fullMessage);
			getLogType(fullMessage, type);

			fullMessage << msg;
		} else {
			getJSONString(fullMessage, name.toCharArray(), msg, type);
		}

		fullMessage << endl;

		(*globalLogFile) << fullMessage;

		if (syncGlobalLog || forceSync) {
			globalLogFile->flush();
		}
	}
}

void Logger::log(const String& msg) const {
	log(msg.toCharArray());
}

void Logger::log(const StringBuffer& msg) const {
	log(msg.toString().toCharArray());
}

void Logger::error(const char* msg) const {
	if (logToConsole) {
		printTime(false, true);

		System::err << " [" << name << "] ERROR - " << msg << endl << flush;
	}

	log(msg, LogLevel::ERROR);
}

void Logger::error(const String& msg) const {
	error(msg.toCharArray());
}

void Logger::error(const StringBuffer& msg) const {
	error(msg.toString());
}

void Logger::fatal(const char* msg) const {
	printTime(false, true);

	System::err << " [" << name << "] FATAL - " << msg << endl;

	log(msg, LogLevel::FATAL, true);

	StackTrace::printStackTrace();

	System::flushStreams();
	System::abort();
}

void Logger::fatal(const String& msg) const {
	fatal(msg.toCharArray());
}

void Logger::fatal(const StringBuffer& msg) const {
	fatal(msg.toString());
}

void Logger::debug(const char* msg) const {
	if (logToConsole && logLevel >= DEBUG) {
		printTime(false);

		System::out << " [" << name << "] DEBUG - " << msg << endl;
	}

	log(msg, LogLevel::DEBUG);
}

void Logger::debug(const String& msg) const {
	debug(msg.toCharArray());
}

void Logger::debug(const StringBuffer& msg) const {
	debug(msg.toString());
}

void Logger::warning(const char* msg) const {
	if (logToConsole) {
		printTime(false);

		System::out << " [" << name << "] WARNING - " << msg << endl;
	}

	log(msg, LogLevel::WARNING);
}

void Logger::warning(const String& msg) const {
	warning(msg.toCharArray());
}

void Logger::warning(const StringBuffer& msg) const {
	warning(msg.toString());
}

void Logger::getLogType(StringBuffer& buffer, LogLevel type) const {
	switch (type) {
		case LogLevel::INFO:
			buffer << "[" << name << "] INFO - ";
			return;
		case LogLevel::DEBUG:
			buffer << "[" << name << "] DEBUG - ";
			return;
		case LogLevel::WARNING:
			buffer << "[" << name << "] WARNING - ";
			return;
		case LogLevel::ERROR:
			buffer << "[" << name << "] ERROR - ";
			return;
		case LogLevel::FATAL:
			buffer << "[" << name << "] FATAL - ";
			return;
		default:
			buffer << "[" << name << "] LOG - ";
			return;
	}
}

const char* Logger::getLogType(LogLevel type)  {
	switch (type) {
		case LogLevel::INFO:
			return "INFO";
		case LogLevel::DEBUG:
			return "DEBUG";
		case LogLevel::WARNING:
			return "WARNING";
		case LogLevel::ERROR:
			return "ERROR";
		case LogLevel::FATAL:
			return "FATAL";
		default:
			return "LOG";
	}

	return "INVALIDLOGLEVEL";
}

void Logger::getTime(StringBuffer& str, bool getFull) {
	Time time;

	uint64 elapsed = Logger::starttime.miliDifference(time);

	if (getFull) {
		str << time.getFormattedTime() << " [" << time.getMiliTime() << " msec] ";
	}

	str << "(" << (elapsed / 1000) << " s)";

	Thread* currentThread = Thread::getCurrentThread();

	if (currentThread != nullptr && getFull)
		str << " " << currentThread->getName() << " - ";
}

void Logger::getTime(String& times, bool getFull) {
	StringBuffer str;

	getTime(str, getFull);

	times = str.toString();
}

void Logger::printTime(bool getFull, bool err) {
	StringBuffer str;

	getTime(str, getFull);

	if (err) {
		System::err << str;
	} else {
		System::out << str;
	}
}

uint64 Logger::getElapsedTime() {
	Time time;
	return Logger::starttime.miliDifference(time) / 1000;
}

String Logger::unescapeJSON(const String& input) {
	enum State {ESCAPED, UNESCAPED};

	State s = UNESCAPED;
	StringBuffer output;

	for (int i = 0; i < input.length(); ++i) {
		switch(s) {
			case ESCAPED:
			{
				switch(input.charAt(i))
				{
					case '"':
						output << '\"';
						break;
					case '/':
						output << '/';
						break;
					case 'b':
						output << '\b';
						break;
					case 'f':
						output << '\f';
						break;
					case 'n':
						output << '\n';
						break;
					case 'r':
						output << '\r';
						break;
					case 't':
						output << '\t';
						break;
					case '\\':
						output << '\\';
						break;
					default:
						output << input.charAt(i);
						break;
				}

				s = UNESCAPED;
				break;
			}
			case UNESCAPED:
			{
				switch(input.charAt(i))
				{
					case '\\':
						s = ESCAPED;
						break;
					default:
						output << input.charAt(i);
						break;
				}
			}
		}
	}

	return output.toString();
}

String Logger::escapeJSON(const String& input) {
	StringBuffer output;

	for (int i = 0; i < input.length(); ++i)  {
		switch (input.charAt(i)) {
			case '"':
				output << "\\\"";
				break;
			case '/':
				output << "\\/";
				break;
			case '\b':
				output << "\\b";
				break;
			case '\f':
				output << "\\f";
				break;
			case '\n':
				output << "\\n";
				break;
			case '\r':
				output << "\\r";
				break;
			case '\t':
				output << "\\t";
				break;
			case '\\':
				output << "\\\\";
				break;
			default:
				output << input.charAt(i);
				break;
		}

	}

	return output.toString();
}

LoggerHelper::LoggerHelper(const Logger& logger, const int logLevel, const bool boolParam)
	: logger(logger), logLevel(logLevel), boolParam(boolParam) {

	if (logLevel == Logger::LogLevel::FATAL) {
		if (boolParam) { //do nothing if assertion in FATAL is true
			willLog = false;

			return;
		}
	} else if (!logger.hasToLog(static_cast<Logger::LogLevel>(logLevel)) && !boolParam) {
		willLog = false;

		return;
	}

	willLog = true;
}

LoggerHelper::LoggerHelper(LoggerHelper&& l)
	: logger(l.logger), logLevel(l.logLevel), boolParam(l.boolParam),
	willLog(l.willLog), buffer(std::move(l.buffer)) {
}

LoggerHelper::~LoggerHelper() {
	flush(false);
}

void LoggerHelper::flush(bool clearBuffer) {
	if (buffer.length() == 0)
		return;

	switch (static_cast<Logger::LogLevel>(logLevel)) {
		case Logger::LogLevel::FATAL:
			logger.fatal(buffer.toString().toCharArray());
			break;
		case Logger::LogLevel::ERROR:
			logger.error(buffer.toString().toCharArray());
			break;
		case Logger::LogLevel::WARNING:
			logger.warning(buffer.toString().toCharArray());
			break;
		case Logger::LogLevel::LOG:
			logger.log(buffer.toString().toCharArray(), Logger::LogLevel::LOG, boolParam);
			break;
		case Logger::LogLevel::INFO:
			logger.info(buffer.toString().toCharArray(), boolParam);
			break;
		case Logger::LogLevel::DEBUG:
			logger.debug(buffer.toString().toCharArray());
			break;
		default:
			logger.fatal("incorrect log level in LoggerHelper");
			break;
	}

	if (clearBuffer) {
		buffer.deleteAll();
	}
}

