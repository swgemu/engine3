/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "Thread.h"

pthread_once_t Thread::initThread = PTHREAD_ONCE_INIT;
pthread_key_t Thread::threadDataKey;

void Thread::initializeThreading() {
	pthread_key_create(&threadDataKey, NULL);
}

void Thread::initializeMainThread(Thread* mainThread) {
	pthread_once(&initThread, initializeThreading);

	pthread_setspecific(threadDataKey, mainThread);
}

void* Thread::executeThread(void* th) {
	pthread_once(&initThread, initializeThreading);

	pthread_setspecific(threadDataKey, th);

	Thread* impl = (Thread*) th;

	impl->run();

	if (impl->isDetached())
		delete impl;

	return NULL;
}

Thread::Thread() {
	pthread_attr_init(&attributes);
}

Thread::~Thread() {
	pthread_attr_destroy(&attributes);
}

void Thread::start() {
	pthread_create(&thread, &attributes, executeThread, this);
}

void Thread::cancel() {
	pthread_cancel(thread);
}

void Thread::kill(int signal) {
	pthread_kill(thread, signal);
}

void Thread::join() {
	if (isDetached())
		setJoinable();

	pthread_join(thread, NULL);
}

void Thread::detach() {
	pthread_detach(thread);
}

void Thread::sleep(uint64 millis) {
	#ifndef PLATFORM_WIN
		struct timespec tm, trem;

		tm.tv_sec = (uint32) millis / 1000;
		tm.tv_nsec = (uint32) (millis % 1000) * 1000000;

		nanosleep(&tm, &trem);
	#else
		Sleep(millis);
	#endif
}

void Thread::sleep(uint64 millis, uint64 nanos) {
	#ifndef PLATFORM_WIN
		struct timespec tm, trem;

		if (millis != 0) {
			tm.tv_sec = (uint32) millis / 1000;
			tm.tv_nsec = (uint32) ((millis % 1000) * 1000000 + nanos);
		} else {
			tm.tv_sec = 0;
			tm.tv_nsec = (uint32) nanos;
		}

		nanosleep(&tm, &trem);
	#else
		//TODO find appropriate method for win32
	#endif
}

bool Thread::isDetached() {
	int state;
	pthread_attr_getdetachstate(&attributes, &state);

	return state == PTHREAD_CREATE_DETACHED;
}

Thread* Thread::getCurrentThread() {
	#ifndef PLATFORM_WIN
		return (Thread*) pthread_getspecific(threadDataKey);
	#else
		return NULL;
	#endif
}

void Thread::setDetached() {
	pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
}

void Thread::setJoinable() {
	pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_JOINABLE);
}

void Thread::setSchedulingPolicy(int policy) {
	pthread_attr_setschedpolicy(&attributes, policy);
}
