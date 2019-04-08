/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
 */

#include "Logger.h"

AtomicReference<FileWriter*> Logger::globalLogFile = nullptr;

Time Logger::starttime;

Logger Logger::console("Console");

volatile int Logger::globalLogLevel = LogLevel::DEBUG;
bool Logger::syncGlobalLog = false;
bool Logger::jsonGlobalLog = false;

Logger::Logger() {
	logFile = nullptr;

	logLevel = LOG;
	doSyncLog = true;
	doGlobalLog = true;
	logTimeToFile = true;
	logLevelToFile = true;
	logJSON = false;
	logToConsole = true;
}

Logger::Logger(const char *s) {
	logFile = nullptr;

	name = s;

	logLevel = LOG;
	doSyncLog = true;
	doGlobalLog = true;
	logTimeToFile = true;
	logLevelToFile = true;
	logJSON = false;
	logToConsole = true;
}

Logger::Logger(const String& s) {
	logFile = nullptr;

	name = s;

	logLevel = LOG;
	doSyncLog = true;
	doGlobalLog = true;
	logTimeToFile = true;
	logLevelToFile = true;
	logJSON = false;
	logToConsole = true;
}

Logger::~Logger() {
	if (logFile != nullptr) {
		closeFileLogger();
		delete logFile;
	}
}

void Logger::setGlobalFileLogger(const char* file) {
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

void Logger::setGlobalFileLogger(const String& file) {
	if (globalLogFile != nullptr)
		closeGlobalFileLogger();

	globalLogFile = new FileWriter(new File(file), true);

	starttime.updateToCurrentTime();
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

		System::out << " [" << name << "] " << msg << "\n";
	}

	log(msg, LogLevel::INFO);
}

void Logger::info(const String& msg, bool forcedLog) const {
	info(msg.toCharArray(), forcedLog);
}

void Logger::info(const StringBuffer& msg, bool forcedLog) const {
	String s = msg.toString();
	info(s, forcedLog);
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

	output << "{\"@timestamp\":\"" << time.getFormattedTimeFull().toCharArray() << "\","
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
	if (logFile == nullptr && globalLogFile == nullptr)
		return;

	//Locker locker(&writeLock);

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

		fullMessage << "\n";

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

		fullMessage << "\n";

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
		printTime(false);

		StringBuffer fullMessage;
		System::out << " [" << name << "] ERROR - " << msg << "\n";

		System::out << fullMessage;
	}

	log(msg, LogLevel::ERROR);
}

void Logger::error(const String& msg) const {
	error(msg.toCharArray());
}

void Logger::error(const StringBuffer& msg) const {
	const String s = msg.toString();
	error(s);
}

void Logger::fatal(const char* msg) const {
	printTime(false);

	char newMsg[256];

	snprintf(newMsg, sizeof(newMsg), " [%s] FATAL - %s\n", name.toCharArray(), msg);
	System::out << newMsg;

	log(msg, LogLevel::FATAL, true);

	abort();
}

void Logger::fatal(const String& msg) const {
	fatal(msg.toCharArray());
}

void Logger::fatal(const StringBuffer& msg) const {
	const String s = msg.toString();
	fatal(s);
}

#ifndef DISABLE_DEBUG_LOG
void Logger::debug(const char* msg) const {
	if (logToConsole && logLevel >= DEBUG) {
		printTime(false);

		StringBuffer fullMessage;
		fullMessage << " [" << name << "] DEBUG - " << msg << "\n";

		System::out << fullMessage;
	}

	//if (logLevel + 1 >= DEBUG)
	log(msg, LogLevel::DEBUG);
}

void Logger::debug(const String& msg) const {
	debug(msg.toCharArray());
}

void Logger::debug(const StringBuffer& msg) const {
	String s = msg.toString();
	debug(s);
}
#endif

void Logger::warning(const char* msg) const {
	if (logToConsole) {
		printTime(false);

		StringBuffer fullMessage;
		fullMessage << " [" << name << "] WARNING - " << msg << "\n";

		System::out << fullMessage;
	}

	log(msg, LogLevel::WARNING);
}

void Logger::warning(const String& msg) const {
	warning(msg.toCharArray());
}

void Logger::warning(const StringBuffer& msg) const {
	String s = msg.toString();
	warning(s);
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
		//str << time.getMiliTime() << " msec ";
		String formattedTime = time.getFormattedTime();
		formattedTime = formattedTime.replaceAll("\n", "");
		str << formattedTime << " [" << time.getMiliTime() << " msec] ";
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

void Logger::printTime(bool getFull) {
	StringBuffer str;

	getTime(str, getFull);

	System::out << str;
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
