/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "Thread.h"

#include <thread>
#include <chrono>

#ifndef PLATFORM_WIN
#include <unistd.h>
#endif

#ifdef PLATFORM_MAC
#include <sched.h>
#endif

#ifdef PLATFORM_WIN
#include <process.h>
#endif

#include "engine/core/Core.h"

namespace ThreadNs {
	static Logger threadLogger("Thread");

	void threadDtor(void*) {

	}

}

std::atomic<int> Thread::threadCounter;
UniqueReference<ThreadInitializer*> Thread::threadInitializer;

pthread_once_t Thread::initThread = PTHREAD_ONCE_INIT;


ThreadLocal<Thread*> Thread::currentThread(ThreadNs::threadDtor);

void Thread::initializeThread(Thread* thread) {
	currentThread.set(thread);
}

void* Thread::executeThread(void* th) {
	const static int vars = Core::initializeProperties("Thread");

	Thread* impl = (Thread*) th;

	currentThread.set(impl);

	if (threadInitializer)
		threadInitializer->onThreadStart(impl);

	impl->run();

	if (threadInitializer)
		threadInitializer->onThreadEnd(impl);

	if (impl->isDetached())
		delete impl;


	return nullptr;
}

//#ifndef PLATFORM_WIN
pid_t Thread::getProcessID() {
#ifdef PLATFORM_WIN
	return _getpid();
#else
	return getpid();
#endif
}
//#endif

Thread::Thread() {
	modifiedObjects = nullptr;

	pthread_attr_init(&attributes);
	memset(&thread, 0, sizeof(thread));

	name = "Thread " + String::valueOf(++threadCounter);
}

Thread::~Thread() {
	pthread_attr_destroy(&attributes);

	if (modifiedObjects) {
		for (auto object : *modifiedObjects) {
			object->release();
		}

		delete modifiedObjects;
		modifiedObjects = nullptr;
	}

	if (deletedFromDatabaseObjects) {
		for (auto object : *deletedFromDatabaseObjects) {
			object->release();
		}

		delete deletedFromDatabaseObjects;
		deletedFromDatabaseObjects = nullptr;
	}
}

void Thread::start() {
	pthread_create(&thread, &attributes, executeThread, this);
}

void Thread::addModifiedObject(DistributedObject* object) {
	if (!modifiedObjects) {
		modifiedObjects = new ModifiedObjectsList();
	}

	modifiedObjects->emplace(object);
	object->acquire();

	const static int maxCount = Core::getIntProperty("Thread.maxModifiedObjects", 3000000);

	ThreadNs::threadLogger.fatal(modifiedObjects->size() < maxCount, "Exceeded Thread.maxModifiedObjects size");
}

void Thread::addDeleteFromDatabaseObject(DistributedObject* object) {
	if (!deletedFromDatabaseObjects) {
		deletedFromDatabaseObjects = new DeleteFromDatabaseObjectsList();
	}

	deletedFromDatabaseObjects->emplace(object);
	object->acquire();

	const static int maxCount = Core::getIntProperty("Thread.maxDeleteFromDatabaseObjects", 3000000);

	ThreadNs::threadLogger.fatal(deletedFromDatabaseObjects->size() < maxCount, "Exceeded Thread.maxDeleteFromDatabaseObjects size");
}

int Thread::getModifiedObjectsCount() const {
	auto modifiedObjects = this->modifiedObjects;

	if (!modifiedObjects) {
		return 0;
	}

	return modifiedObjects->size();
}

int Thread::getDeleteFromDatabaseObjectsCount() const {
	auto modifiedObjects = this->deletedFromDatabaseObjects;

	if (!modifiedObjects) {
		return 0;
	}

	return modifiedObjects->size();
}

Thread::DeleteFromDatabaseObjectsList* Thread::takeDeleteFromDatabaseObjects() {
	auto copy = deletedFromDatabaseObjects;
	deletedFromDatabaseObjects = nullptr;

	return copy;
}

Thread::ModifiedObjectsList* Thread::takeModifiedObjects() {
	auto copy = modifiedObjects;
	modifiedObjects = nullptr;

	return copy;
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

	pthread_join(thread, nullptr);
}

void Thread::detach() {
	pthread_detach(thread);
}

void Thread::sleep(uint64 millis) {
	std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

void Thread::sleep(uint64 millis, uint64 nanos) {
	std::this_thread::sleep_for(std::chrono::milliseconds(millis) + std::chrono::nanoseconds(nanos));
}

void Thread::yield() {
	std::this_thread::yield();
}

bool Thread::isDetached() {
	int state;
	pthread_attr_getdetachstate(&attributes, &state);

	return state == PTHREAD_CREATE_DETACHED;
}

Thread* Thread::getCurrentThread() {
	return currentThread.get();
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

void Thread::assignToCPU(int cpu) {
//	Disabling this, new kernels are smart enough usually
	/*
#ifndef VERSION_PUBLIC
#ifdef PLATFORM_LINUX
	cpu_set_t cpuSet;

	CPU_ZERO(&cpuSet);
	CPU_SET(cpu, &cpuSet);

	if (sched_setaffinity(0, sizeof(cpuSet), &cpuSet) < 0) {
		perror("sched_setaffinity");
	}
#endif
#endif*/
}
