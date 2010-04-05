/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef THREAD_H_
#define THREAD_H_

#include "system/platform.h"

#include <pthread.h>

#include <signal.h>

#include "system/lang/Runnable.h"

#include "system/lang/String.h"

#include "atomic/AtomicInteger.h"

#include "ThreadLocal.h"

namespace sys {
  namespace thread {

	/*!
	 * thread wrapper class. the inheriting classes must implement a run() method that have to contain the code to be executed
	 */
	class Thread : public Runnable {
		pthread_t thread;

		pthread_attr_t attributes;

		String name;

		static AtomicInteger threadCounter;

		static pthread_once_t initThread;

		static ThreadLocal<Thread> currentThread;

	public:
		//! allocates a new Thread
		Thread();
		virtual ~Thread();

	#ifndef PLATFORM_WIN
		static pid_t getProcessID();
	#endif

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

		const String& getName() {
			return name;
		}

	protected:
		static void* executeThread(void* th);

		//! Initializes main thread local key
		static void initializeThread(Thread* thread);

		friend class Core;
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*THREAD_H_*/
