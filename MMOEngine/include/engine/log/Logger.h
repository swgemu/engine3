/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LOGGER_H_
#define LOGGER_H_

#include "../../system/lang.h"

namespace engine {
  namespace log {

	class Logger {
		string name;
		
		static ofstream* globallogfile;
		ofstream* logfile;
	
		static Time starttime;
		
		bool doLog, doGlobalLog;

	public:
		static Logger console;
		
	public:
		Logger();
		Logger(const char *s);	
		Logger(const string& s);	
		
		~Logger();
	
		static void setGlobalFileLogger(const char* file);
		static void setGlobalFileLogger(const string& file);
	
		void setFileLogger(const char* file);
		void setFileLogger(const string& file);
		
		static void closeGlobalFileLogger();
		void closeFileLogger();
	
		void info(const char *msg, bool forcedLog = false);
		void info(const string& msg, bool forcedLog = false);
		void info(const stringstream& msg, bool forcedLog = false);
	
		void log(const char *msg);
		void log(const string& msg);
		void log(const stringstream& msg);
	
		void error(const char* msg);
		void error(const string& msg);
		void error(const stringstream& msg);
	
		static void getTime(string& time, bool getFull = true);
		static void printTime(bool getFull = true);
		
		static uint64 getElapsedTime();

		// setters		
		inline void setLogging(bool doLog) {
			Logger::doLog = doLog;
		}

		inline void setGlobalLogging(bool doLog) {
			doGlobalLog = doLog;
		}
	
		inline void setLoggingName(const string& s) {
			name = s;
		}
		
		// getters
		inline string& getLoggingName() {
			return name;
		}
		 
	};

  } // namespace log
} // namespace engine

using namespace engine::log;

#endif /*LOGGER_H_*/
