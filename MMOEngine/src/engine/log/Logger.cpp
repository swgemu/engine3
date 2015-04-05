/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
 */

#include "Logger.h"

AtomicReference<FileWriter*> Logger::globalLogFile = NULL;

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

void Logger::setFileLogger(const String& file, bool appendData) {
	if (logFile != NULL)
		closeFileLogger();

	File* fileObject = new File(file);

	logFile = new FileWriter(fileObject, appendData);
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

void Logger::info(const char *msg, bool forcedLog) const {
	if (logLevel >=  INFO || forcedLog) {
		printTime(false);

		System::out << " [" << name << "] " << msg << "\n";
	}

	if (logLevel + 1 >= INFO || forcedLog)
		log(msg);
}

void Logger::info(const String& msg, bool forcedLog) const {
	info(msg.toCharArray(), forcedLog);
}

void Logger::info(const StringBuffer& msg, bool forcedLog) const {
	String s = msg.toString();
	info(s, forcedLog);
}

void Logger::log(const char *msg) const {
	if (logFile == NULL && globalLogFile == NULL)
		return;

	//Locker locker(&writeLock);

	if (logLevel > LOG && logFile != NULL) {
		FileWriter* logFile = const_cast<FileWriter*>(this->logFile);

		String time;
		getTime(time);

		(*logFile) << time << " [" << name << "] " << msg << "\n";

		logFile->flush();
	} else if (doGlobalLog && globalLogFile != NULL) {
		FileWriter* globalLogFile = const_cast<FileWriter*>(this->globalLogFile.get());

		String time;
		getTime(time);

		(*globalLogFile) << time << " [" << name << "] " << msg << "\n";

		globalLogFile->flush();
		
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

	System::out << " [" << name << "] ERROR - " << msg << "\n";

	log(msg);
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

	System::out << " [" << name << "] FATAL - " << msg << "\n";

	log(msg);

	abort();
}

void Logger::fatal(const String& msg) const {
	fatal(msg.toCharArray());
}

void Logger::fatal(const StringBuffer& msg) const {
	String s = msg.toString();
	fatal(s);
}

void Logger::debug(const char* msg) const {
	if (logLevel >= DEBUG) {
		printTime(true);

		System::out << " [" << name << "] DEBUG - " << msg << "\n";
	}

	if (logLevel + 1 >= DEBUG)
		log(msg);
}

void Logger::debug(const String& msg) const {
	debug(msg.toCharArray());
}

void Logger::debug(const StringBuffer& msg) const {
	String s = msg.toString();
	debug(s);
}

void Logger::warning(const char* msg) const {
	printTime(false);

	System::out << " [" << name << "] WARNING - " << msg << "\n";

	log(msg);
}

void Logger::warning(const String& msg) const {
	warning(msg.toCharArray());
}

void Logger::warning(const StringBuffer& msg) const {
	String s = msg.toString();
	warning(s);
}

void Logger::getTime(String& times, bool getFull) {
	Time time;
	StringBuffer str;

	uint64 elapsed = Logger::starttime.miliDifference(time);

	if (getFull) {
		//str << time.getMiliTime() << " msec ";
		String formattedTime = time.getFormattedTime();
		formattedTime = formattedTime.replaceAll("\n", "");
		str << formattedTime << " [" << time.getMiliTime() << " msec] ";
	}

	str << "(" << (elapsed / 1000) << " s)";

	Thread* currentThread = Thread::getCurrentThread();

	if (currentThread != NULL && getFull)
		str << " " << currentThread->getName() << " -";

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
		System::out << formattedTime << " [" << time.getMiliTime() << " msec] ";
	}

	System::out << "(" << (elapsed / 1000) << " s)";

	Thread* currentThread = Thread::getCurrentThread();

	if (currentThread != NULL && getFull)
		str << " " << currentThread->getName() << " -";
}

uint64 Logger::getElapsedTime() {
	Time time;
	return Logger::starttime.miliDifference(time) / 1000;
}
