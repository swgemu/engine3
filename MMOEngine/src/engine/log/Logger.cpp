/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "Logger.h"

AtomicReference<FileWriter> Logger::globalLogFile = NULL;

Time Logger::starttime;

Logger Logger::console("Console");

Logger::Logger() {
	logFile = NULL;

	logLevel = LOG;
	doGlobalLog = true;
}

Logger::Logger(const char *s) {
	logFile = NULL;

	name = s;

	logLevel = LOG;
	doGlobalLog = true;
}

Logger::Logger(const String& s) {
	logFile = NULL;

	name = s;

	logLevel = LOG;
	doGlobalLog = true;
}

Logger::~Logger() {
	if (logFile != NULL) {
		closeFileLogger();
		delete logFile;
	}
}

void Logger::setGlobalFileLogger(const char* file) {
	globalLogFile = new FileWriter(new File(file));

	starttime.updateToCurrentTime();
}

void Logger::setGlobalFileLogger(const String& file) {
	if (globalLogFile != NULL)
		closeGlobalFileLogger();

	globalLogFile = new FileWriter(new File(file));

	starttime.updateToCurrentTime();
}

void Logger::setFileLogger(const char* file) {
	if (logFile != NULL)
		closeFileLogger();

	logFile = new FileWriter(new File(file));
}

void Logger::setFileLogger(const String& file) {
	//logFile = new FileWriter(new File(file));
}

void Logger::closeGlobalFileLogger() {
	if (globalLogFile != NULL) {
		globalLogFile->close();

		delete globalLogFile;
		globalLogFile = NULL;
	}
}

void Logger::closeFileLogger() {
	if (logFile != NULL) {
		logFile->close();

		delete logFile;
		logFile = NULL;
	}
}

void Logger::info(const char *msg, bool forcedLog) {
	if (logLevel >=  INFO || forcedLog) {
		printTime(false);

		System::out << " [" << name << "] " << msg << "\n";
	}

	if (logLevel + 1 >= INFO || forcedLog)
		log(msg);
}

void Logger::info(const String& msg, bool forcedLog) {
	info(msg.toCharArray(), forcedLog);
}

void Logger::info(const StringBuffer& msg, bool forcedLog) {
	String s = msg.toString();
	info(s, forcedLog);
}

void Logger::log(const char *msg) {
	if (logLevel > LOG && logFile != NULL) {
		String time;
		getTime(time);

		(*logFile) << time << " [" << name << "] " << msg << "\n";

		logFile->flush();
	} else if (doGlobalLog && globalLogFile != NULL) {
		String time;
		getTime(time);

		(*globalLogFile) << time << " [" << name << "] " << msg << "\n";

		globalLogFile->flush();
	}
}

void Logger::log(const String& msg) {
	log(msg.toCharArray());
}

void Logger::log(const StringBuffer& msg) {
	log(msg.toString().toCharArray());
}

void Logger::error(const char* msg) {
	printTime(false);

	System::out << " [" << name << "] ERROR - " << msg << "\n";

	log(msg);
}

void Logger::error(const String& msg) {
	error(msg.toCharArray());
}

void Logger::error(const StringBuffer& msg) {
	String s = msg.toString();
	error(s);
}

void Logger::debug(const char* msg) {
	if (logLevel >= DEBUG) {
		printTime(true);

		System::out << " [" << name << "] DEBUG - " << msg << "\n";
	}

	if (logLevel + 1 >= DEBUG)
		log(msg);
}

void Logger::debug(const String& msg) {
	debug(msg.toCharArray());
}

void Logger::debug(const StringBuffer& msg) {
	String s = msg.toString();
	debug(s);
}

void Logger::warning(const char* msg) {
	printTime(false);

	System::out << " [" << name << "] WARNING - " << msg << "\n";

	log(msg);
}

void Logger::warning(const String& msg) {
	warning(msg.toCharArray());
}

void Logger::warning(const StringBuffer& msg) {
	String s = msg.toString();
	warning(s);
}

void Logger::getTime(String& times, bool getFull) {
	Time time;
	StringBuffer str;

	uint64 elapsed = Logger::starttime.miliDifference(time);

	if (getFull)
		str << time.getMiliTime() << " msec ";

	str << "(" << (elapsed / 1000) << " s)";

	if (getFull)
		str << " " << Thread::getCurrentThread()->getName() << " -";

	times = str.toString();
}

void Logger::printTime(bool getFull) {
	Time time;
	StringBuffer str;

	uint64 elapsed = Logger::starttime.miliDifference(time);

	if (getFull)
		System::out << time.getMiliTime() << " msec ";

	System::out << "(" << (elapsed / 1000) << " s)";

	if (getFull)
		System::out << " " << Thread::getCurrentThread()->getName() << " -";
}

uint64 Logger::getElapsedTime() {
	Time time;
	return Logger::starttime.miliDifference(time) / 1000;
}
