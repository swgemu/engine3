/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "Logger.h"

ofstream* Logger::globallogfile = NULL;

Time Logger::starttime;

Logger Logger::console("Console");

Logger::Logger() {
	logfile = NULL;

	doLog = true;
	doGlobalLog = true;
}

Logger::Logger(const char *s) {
	logfile = NULL;

	name = s;

	doLog = true;
	doGlobalLog = true;
}

Logger::Logger(const string& s) {
	logfile = NULL;

	name = s;

	doLog = true;
	doGlobalLog = true;
}

Logger::~Logger() {
	if (logfile != NULL) {
		closeFileLogger();
		delete logfile;
	}
}

void Logger::setGlobalFileLogger(const char* file) {
	globallogfile = new ofstream();
	globallogfile->open(file);
		
	starttime.update();
}

void Logger::setGlobalFileLogger(const string& file) {
	if (globallogfile != NULL)
		closeGlobalFileLogger();
	
	globallogfile = new ofstream();
	globallogfile->open(file.c_str());

	starttime.update();
}

void Logger::setFileLogger(const char* file) {
	if (logfile != NULL)
		closeFileLogger();
	
	logfile = new ofstream();
	logfile->open(file);
}

void Logger::setFileLogger(const string& file) {
	logfile = new ofstream();
	logfile->open(file.c_str());
}

void Logger::closeGlobalFileLogger() {
	if (globallogfile != NULL) {
		globallogfile->close();

		delete globallogfile;
		globallogfile = NULL;
	}
}

void Logger::closeFileLogger() {
	if (logfile != NULL) {
		logfile->close();

		delete logfile;
		logfile = NULL;
	}
}

void Logger::info(const char *msg, bool forcedLog) {
	if (doLog || forcedLog) {
		printTime(false);
		cout << " [" << name << "] " << msg << "\n";
	}
			
	log(msg);
}

void Logger::info(const string& msg, bool forcedLog) {
	info(msg.c_str(), forcedLog);
}

void Logger::info(const stringstream& msg, bool forcedLog) {
	string s = msg.str();
	info(s, forcedLog);
}

void Logger::log(const char *msg) {
	if (doLog && logfile != NULL) {
		string time;
		getTime(time);
		
		(*logfile) << time; 
		(*logfile) << " [";
		(*logfile) << name; 
		(*logfile) << "] "; 
		(*logfile) << msg; 
		(*logfile) << "\n";
		
		logfile->flush();
	} else if (doGlobalLog && globallogfile != NULL) {
		string time;
		getTime(time);

		(*globallogfile) << time << " [" << name << "] " << msg << "\n";
		
		globallogfile->flush();
	} 
}

void Logger::log(const string& msg) {
	log(msg.c_str());
}

void Logger::log(const stringstream& msg) {
	log(msg.str().c_str());
}

void Logger::error(const char* msg) {
	printTime(false);
	
	cout << " [" << name << "] ERROR - " << msg << "\n";

	log(msg);
}

void Logger::error(const string& msg) {
	error(msg.c_str());
}

void Logger::error(const stringstream& msg) {
	string s = msg.str();
	error(s);
}	

void Logger::getTime(string& times, bool getFull) {
	Time time;
	stringstream str;
		
	uint64 elapsed = Logger::starttime.miliDifference(time);
	
	if (getFull)
		str << time.getMiliTime() << " msec ";
			
	str << "(" << (elapsed / 1000) << " s)";

	times = str.str();
}

void Logger::printTime(bool getFull) {
	Time time;
	stringstream str;
		
	uint64 elapsed = Logger::starttime.miliDifference(time);
	
	if (getFull)
		cout << time.getMiliTime() << " msec ";
			
	cout << "(" << (elapsed / 1000) << " s)";
}

uint64 Logger::getElapsedTime() {
	Time time;
	return Logger::starttime.miliDifference(time) / 1000;
}
