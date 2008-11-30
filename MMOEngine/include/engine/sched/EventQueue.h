/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef EVENTQUEUE_H_
#define EVENTQUEUE_H_

//#define TRACE_EVENTS

#include "../../system/lang.h"
#include "../log/Logger.h"

#include "Event.h"

namespace engine {
  namespace sched {

	class EventQueue : public AdvancedBinaryHeap, public Condition, public Logger {
		Mutex* condMutex;
	
		bool blocked;
	
		bool waitingForEvent;
		bool changePlan;
		
	public:
		EventQueue();
		
		~EventQueue();
		
		void add(Event* e, uint64 time);
		void add(Event* e, Time& time);
		
		void add(Event* e, bool doLock = true);
		
		Event* get();
		
		bool remove(Event* e, bool doLock = true);
	
		void flush();
	
		void repair();
		
		void printQueue();
		
		// setters and getters
		inline void setMutexName(const String& s) {
			condMutex->setLockName(s);
		}
	};

  } // namespace sched
} // namespace engine

using namespace engine::sched;

#endif /*EVENTQUEUE_H_*/
