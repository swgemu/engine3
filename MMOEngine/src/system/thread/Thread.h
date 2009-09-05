/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef THREAD_H_
#define THREAD_H_

#include "../platform.h"

#include <pthread.h>

#include <signal.h>

#include "../lang/Runnable.h"

namespace sys {
  namespace thread {

	/*!
	 * thread wrapper class. the inheriting classes must implement a run() method that have to contain the code to be executed
	 */
	class Thread : public Runnable {
		pthread_t thread;

		pthread_attr_t attributes;

		static pthread_once_t initThread;
		static pthread_key_t threadDataKey;

	private:
		static void initializeThreading();

		static void* executeThread(void* th);

	public:
		//! allocates a new Thread
		Thread();
		virtual ~Thread();

		//! Initializes main thread local key
		static void initializeMainThread(Thread* mainThread);

		//! causes this thread to begin execution
		void start();

		//! causes this thread to be cancelled
		void cancel();

		//! causes this thread to be killed
		void kill(int signal = SIGINT);

		//! causes the calling thread to be waiting until this thread finishes
		void join();

		void detach();

		static void sleep(uint64 millis);

		static void sleep(uint64 millis, uint64 nanos);

		bool isDetached();

		static Thread* getCurrentThread();

		// setters
		void setDetached();

		void setJoinable();

		void setSchedulingPolicy(int policy);

	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*THREAD_H_*/
