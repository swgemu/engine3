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

Logger::Logger() {
	logFile = nullptr;

	logLevel = LOG;
	doSyncLog = true;
	doGlobalLog = true;
	logTimeToFile = true;
	logLevelToFile = true;
}

Logger::Logger(const char *s) {
	logFile = nullptr;

	name = s;

	logLevel = LOG;
	doSyncLog = true;
	doGlobalLog = true;
	logTimeToFile = true;
	logLevelToFile = true;
}

Logger::Logger(const String& s) {
	logFile = nullptr;

	name = s;

	logLevel = LOG;
	doSyncLog = true;
	doGlobalLog = true;
	logTimeToFile = true;
	logLevelToFile = true;
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
};

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
	if (globalLogFile != nullptr) {
		globalLogFile->close();

		delete globalLogFile->getFile();

		delete globalLogFile;
		globalLogFile = nullptr;
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
	if (logLevel >= INFO || forcedLog) {
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

void Logger::log(const char *msg, LogLevel type) const {
	if (logFile == nullptr && globalLogFile == nullptr)
		return;

	//Locker locker(&writeLock);

	if (logLevel >= type && logFile != nullptr) {
		FileWriter* logFile = const_cast<FileWriter*>(this->logFile);

		StringBuffer fullMessage;

		if (logTimeToFile) {
			getTime(fullMessage);
		}

		if (logLevelToFile) {
			getLogType(fullMessage, type);
		}

		fullMessage << msg << "\n";

		(*logFile) << fullMessage;

		if (doSyncLog) {
			logFile->flush();
		}
	} else if (doGlobalLog && globalLogFile != nullptr && globalLogLevel >= type) {
		FileWriter* globalLogFile = const_cast<FileWriter*>(this->globalLogFile.get());

		StringBuffer fullMessage;

		getTime(fullMessage);
		getLogType(fullMessage, type);

		fullMessage << msg << "\n";

		(*globalLogFile) << fullMessage;

		if (syncGlobalLog) {
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
	printTime(false);

	StringBuffer fullMessage;
	fullMessage << " [" << name << "] ERROR - " << msg << "\n";

	System::out << fullMessage;

	log(msg, LogLevel::ERROR);
}

void Logger::error(const String& msg) const {
	error(msg.toCharArray());
}

void Logger::error(const StringBuffer& msg) const {
	String s = msg.toString();
	error(s);
}

void Logger::fatal(const char* msg) const {
	printTime(false);

	StringBuffer fullMessage;
	fullMessage << " [" << name << "] FATAL - " << msg << "\n";

	System::out << fullMessage;

	log(msg, LogLevel::FATAL);

	abort();
}

void Logger::fatal(const String& msg) const {
	fatal(msg.toCharArray());
}

void Logger::fatal(const StringBuffer& msg) const {
	String s = msg.toString();
	fatal(s);
}

#ifndef DISABLE_DEBUG_LOG
void Logger::debug(const char* msg) const {
	if (logLevel >= DEBUG) {
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
	printTime(false);

	StringBuffer fullMessage;
	fullMessage << " [" << name << "] WARNING - " << msg << "\n";

	System::out << fullMessage;

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
			buffer << " [" << name << "] INFO - ";
			return;
		case LogLevel::DEBUG:
			buffer << " [" << name << "] DEBUG - ";
			return;
		case LogLevel::WARNING:
			buffer << " [" << name << "] WARNING - ";
			return;
		case LogLevel::ERROR:
			buffer << " [" << name << "] ERROR - ";
			return;
		case LogLevel::FATAL:
			buffer << " [" << name << "] FATAL - ";
			return;
		default:
			buffer << " [" << name << "] LOG - ";
			return;
	}
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
		str << " " << currentThread->getName() << " -";
}

void Logger::getTime(String& times, bool getFull) {
	StringBuffer str;

	getTime(str, getFull);

	times = str.toString();
}

void Logger::printTime(bool getFull) {
	Time time;
	StringBuffer str;

	uint64 elapsed = Logger::starttime.miliDifference(time);

	if (getFull) {
		//System::out << time.getMiliTime() << " msec ";
		String formattedTime = time.getFormattedTime();
		formattedTime = formattedTime.replaceAll("\n", "");
		str << formattedTime << " [" << time.getMiliTime() << " msec] ";
	}

	str << "(" << (elapsed / 1000) << " s)";

	Thread* currentThread = Thread::getCurrentThread();

	if (currentThread != nullptr && getFull)
		str << " " << currentThread->getName() << " -";

	System::out << str;
}

uint64 Logger::getElapsedTime() {
	Time time;
	return Logger::starttime.miliDifference(time) / 1000;
}
