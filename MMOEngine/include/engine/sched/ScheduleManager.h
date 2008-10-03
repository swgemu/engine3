/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SCHEDULEMANAGER_H_
#define SCHEDULEMANAGER_H_

#include "../../system/lang.h"

#include "../log/Logger.h"

#include "../util/Singleton.h"

#include "Event.h"
#include "EventQueue.h"

namespace engine {
  namespace sched {

	class ScheduleManager : public Singleton<ScheduleManager>,  public Thread, public Mutex, public Logger {
		EventQueue events;
		
		bool doRun;
	
	public:
		ScheduleManager();
		ScheduleManager(const string& s);
	
		virtual ~ScheduleManager();
	
		void start();
		
		void run();
	
		void stop();
	
		inline void addEvent(Event* event, uint64 time = 0) {
			events.add(event, time);
		}
	
		inline void addEvent(Event* event, Time& time) {
			events.add(event, time);
		}
	
		inline void deleteEvent(Event* event) {
			events.remove(event);
		}
	
		void fixQueue() {
			events.repair();
		}
		
		inline int getQueueSize() {
			return events.size();
		}
		
		inline void printEvents() {
			events.printQueue();
		}
	
		friend class SingletonWrapper<ScheduleManager>;
	};

  } // namespace sched
} // namespace engine

using namespace engine::sched;

#endif /*SCHEDULEMANAGER_H_*/
