/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "Logger.h"

FileWriter* Logger::globalLogFile = NULL;

Time Logger::starttime;

Logger Logger::console("Console");

Logger::Logger() {
	logFile = NULL;

	doLog = true;
	doGlobalLog = true;
}

Logger::Logger(const char *s) {
	logFile = NULL;

	name = s;

	doLog = true;
	doGlobalLog = true;
}

Logger::Logger(const String& s) {
	logFile = NULL;

	name = s;

	doLog = true;
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

	starttime.update();
}

void Logger::setGlobalFileLogger(const String& file) {
	if (globalLogFile != NULL)
		closeGlobalFileLogger();

	globalLogFile = new FileWriter(new File(file));

	starttime.update();
}

void Logger::setFileLogger(const char* file) {
	if (logFile != NULL)
		closeFileLogger();

	logFile = new FileWriter(new File(file));
}

void Logger::setFileLogger(const String& file) {
	logFile = new FileWriter(new File(file));
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
	if (doLog || forcedLog) {
		printTime(false);
		System::out << " [" << name << "] " << msg << "\n";
	}

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
	if (doLog && logFile != NULL) {
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

void Logger::getTime(String& times, bool getFull) {
	Time time;
	StringBuffer str;

	uint64 elapsed = Logger::starttime.miliDifference(time);

	if (getFull)
		str << time.getMiliTime() << " msec ";

	str << "(" << (elapsed / 1000) << " s)";

	times = str.toString();
}

void Logger::printTime(bool getFull) {
	Time time;
	StringBuffer str;

	uint64 elapsed = Logger::starttime.miliDifference(time);

	if (getFull)
		System::out << time.getMiliTime() << " msec ";

	System::out << "(" << (elapsed / 1000) << " s)";
}

uint64 Logger::getElapsedTime() {
	Time time;
	return Logger::starttime.miliDifference(time) / 1000;
}
