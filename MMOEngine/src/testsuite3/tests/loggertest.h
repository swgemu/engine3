/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * loggertest.h
 *
 *  Created on: 18/03/2012
 *      Author: victor
 */

#ifndef LOGGERTEST_H_
#define LOGGERTEST_H_

#include "engine/log/Logger.h"

class LoggerTest : public Logger {
public:
	LoggerTest() {
		setLogging(true);
		setLoggingName("LoggerTest");
		setFileLogger("ebati.log", true);
	}
};

void loggertest() {
	LoggerTest test;

	for (int i = 0; i < 10; ++i)
		test.info("logging " + String::valueOf(i), true);
}


#endif /* LOGGERTEST_H_ */
