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

namespace engine {
	namespace core {
		class Core;
	}
}

namespace sys {
  namespace thread {

  	 class Lockable;

  	 class LockableTrace {
  	 public:
  		 Lockable* lockable;
  		 byte locked;
  		 Lockable* crossedTo;
  		 StackTrace trace;
  		 bool monitorLike;

  		 LockableTrace(Lockable* l, byte value, Lockable* cross = NULL, bool monitor = false) :
  			lockable(l), locked(value), crossedTo(cross), monitorLike(monitor) {
  		 }

  		 LockableTrace(const LockableTrace& l) : lockable(l.lockable), locked(l.locked),
  				 crossedTo(l.crossedTo), trace(l.trace), monitorLike(l.monitorLike) {
  		 }

  		LockableTrace& operator=(const LockableTrace& l) {
  			if (this == &l)
  				return *this;

  			lockable = l.lockable;
  			locked = l.locked;
  			crossedTo = l.crossedTo;
  			monitorLike = l.monitorLike;
  			trace = l.trace;

  			return *this;
  		}
  	 };

	/*!
	 * thread wrapper class. the inheriting classes must implement a run() method that have to contain the code to be executed
	 */
	class Thread : public Runnable {
		pthread_t thread;

		pthread_attr_t attributes;

		String name;

		static AtomicInteger threadCounter;

		static pthread_once_t initThread;

		static ThreadLocal<Thread*> currentThread;

		//only used in testing
		ArrayList<Lockable*> acquiredLockables;
		ArrayList<LockableTrace> lockableTrace;

	public:
		//! allocates a new Thread
		Thread();
		virtual ~Thread();

		static pid_t getProcessID();

		//! causes this thread to begin execution
		virtual void start();

		//! causes this thread to be cancelled
		void cancel();

		//! causes this thread to be killed
		void kill(int signal = SIGINT);

		//! causes the calling thread to be waiting until this thread finishes
		void join();

		void detach();

		static void sleep(uint64 millis);

		static void sleep(uint64 millis, uint64 nanos);

		static void yield();

		bool isDetached();

		static Thread* getCurrentThread();

		// setters
		void setDetached();

		void setJoinable();

		void setSchedulingPolicy(int policy);

		const String& getName() {
			return name;
		}

		ArrayList<Lockable*>* getAcquiredLockables() {
			return &acquiredLockables;
		}

		ArrayList<LockableTrace>* getLockableTrace() {
			return &lockableTrace;
		}

		void addAcquiredLockable(Lockable* lockable, Lockable* cross = NULL, bool monitorLike = false, bool addToTrace = true) {
			acquiredLockables.add(lockable);

			if (addToTrace)
				lockableTrace.add(LockableTrace(lockable, 1, cross, monitorLike));
		}

		void removeAcquiredLockable(Lockable* lockable) {
			acquiredLockables.removeElement(lockable);

			lockableTrace.add(LockableTrace(lockable, 0));
		}

	protected:
		static void* executeThread(void* th);

		//! Initializes main thread local key
		static void initializeThread(Thread* thread);

		friend class engine::core::Core;
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*THREAD_H_*/
