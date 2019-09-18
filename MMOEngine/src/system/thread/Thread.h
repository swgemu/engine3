/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef THREAD_H_
#define THREAD_H_

#include "system/platform.h"

#include <pthread.h>
#include <atomic>
#include <signal.h>

#include "system/lang/Runnable.h"
#include "system/lang/String.h"
#include "system/lang/ref/UniqueReference.h"

#include "atomic/AtomicInteger.h"

#include "ThreadLocal.h"

#include "engine/util/bytell_hash_map.hpp"

namespace engine {
	namespace core {
		class Core;

		class TaskWorkerThread;
	}
}

namespace sys {
  namespace lang {
  	class Object;
  }

  namespace util {
	 template<class T> class SortedVector;
  }

  namespace thread {

  	 class Lockable;

  	 class LockableTrace {
  	 public:
  		 Lockable* lockable;
  		 byte locked;
  		 Lockable* crossedTo;
  		 StackTrace trace;
  		 bool monitorLike;

  		 LockableTrace(Lockable* l, byte value, Lockable* cross = nullptr, bool monitor = false) :
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

 	class Thread;

	class ThreadInitializer {
	public:
		virtual ~ThreadInitializer() {
		}

		virtual void onThreadStart(Thread* thread) = 0;
		virtual void onThreadEnd(Thread* thread) = 0;
	};
	/*!
	 * thread wrapper class. the inheriting classes must implement a run() method that have to contain the code to be executed
	 */
	class Thread : public Runnable {
		pthread_t thread;
		pthread_attr_t attributes;

		String name;

		static std::atomic<int> threadCounter;
		static pthread_once_t initThread;

		static ThreadLocal<Thread*> currentThread;
		static UniqueReference<ThreadInitializer*> threadInitializer;

	protected:
		using ModifiedObjectsList = ArrayList<Object*>;//ska::bytell_hash_set<void*>;
		using DeleteFromDatabaseObjectsList = ArrayList<Object*>;
		//only used in testing
		ArrayList<Lockable*> acquiredLockables;
		ArrayList<LockableTrace> lockableTrace;
		ModifiedObjectsList* modifiedObjects = nullptr;
		DeleteFromDatabaseObjectsList* deletedFromDatabaseObjects = nullptr;

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

		//does nothing on osx
		void assignToCPU(int cpu);

		static void setThreadInitializer(ThreadInitializer* init) {
			threadInitializer = init;
		}

		static ThreadInitializer* getThreadInitializer() {
			return threadInitializer;
		}

		const String& getName() {
			return name;
		}

		ArrayList<Lockable*>* getAcquiredLockables() {
			return &acquiredLockables;
		}

		ArrayList<LockableTrace>* getLockableTrace() {
			return &lockableTrace;
		}

		void addModifiedObject(Object* object);
		void addDeleteFromDatabaseObject(Object* object);

		void addAcquiredLockable(Lockable* lockable, Lockable* cross = nullptr, bool monitorLike = false, bool addToTrace = true) {
			acquiredLockables.add(lockable);

			if (addToTrace)
				lockableTrace.add(LockableTrace(lockable, 1, cross, monitorLike));
		}

		void removeAcquiredLockable(Lockable* lockable) {
			acquiredLockables.removeElement(lockable);

			lockableTrace.add(LockableTrace(lockable, 0));
		}

		virtual engine::core::TaskWorkerThread* asTaskWorkerThread() {
			return nullptr;
		}

		ModifiedObjectsList* takeModifiedObjects();
		DeleteFromDatabaseObjectsList* takeDeleteFromDatabaseObjects();

		int getModifiedObjectsCount() const;
		int getDeleteFromDatabaseObjectsCount() const;

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
