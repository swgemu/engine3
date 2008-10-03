/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ServiceThread.h"

ServiceThread::ServiceThread(const string& s) : Thread(), Mutex(s + "Thread"), Logger(s) {
	scheduler = new ScheduleManager(s + "Scheduler");
	usingOwnScheduler = true;
	
	setRunning(false);
}

ServiceThread::ServiceThread(const string& s, ScheduleManager* sched) : Thread(), Mutex(s + "Thread"), Logger(s) {
	scheduler = sched;
	usingOwnScheduler = false;
	
	setRunning(false);
}

ServiceThread::~ServiceThread() {
	if (usingOwnScheduler && scheduler != NULL) {
		delete scheduler;
		scheduler = NULL;
	}
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
		while (!isReady());
}


void ServiceThread::run() {
	setRunning(true);
}

void ServiceThread::stop(bool doJoin) {
	if (isRunning()) {
		setRunning(false);
		
		if (doJoin) {
			join();
		} else {
			kill(0);
			Thread::sleep(10);
		}
	} else
		throw ServiceException("unable to stop not running service");
}
