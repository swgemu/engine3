/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/core/Core.h"

#include "ServiceThread.h"

ServiceThread::ServiceThread(const String& s) : Thread(), Mutex(s + "Thread"), Logger(s) {
	setRunning(false);
}

ServiceThread::~ServiceThread() {
}

void ServiceThread::init() {
}

void ServiceThread::start(bool waitForStartup) {
	if (isRunning())
		throw ServiceException("service already running");

	setRunning(true);

	if (waitForStartup)
		setReady(true);

	Thread::start();

	if (waitForStartup)
		while (!isReady()) ;
}


void ServiceThread::run() {
	setRunning(true);
}

void ServiceThread::stop(bool doJoin) {
	if (isRunning()) {
		setRunning(false);
	}

	if (doJoin) {
		join();
	} else {
		kill(0);
		Thread::sleep(10);
	}
	 /*else
		throw ServiceException("unable to stop not running service");*/
}
