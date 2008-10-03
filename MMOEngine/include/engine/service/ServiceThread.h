/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SERVICETHREAD_H_
#define SERVICETHREAD_H_

#include "../../system/lang.h"

#include "../log/Logger.h"

#include "../sched/ScheduleManager.h"

#include "ServiceException.h"

namespace engine {
  namespace service {
  	
	class ServiceThread : public Thread, public Mutex, public Logger {
	protected:
		ScheduleManager* scheduler;
		bool usingOwnScheduler;
		
		bool doRun, serviceReady;
	
	public:
		ServiceThread(const string& s);
		ServiceThread(const string& s, ScheduleManager* sched);
		
		virtual ~ServiceThread();
		
		virtual void init();

		void start(bool waitForStartup = true);
	
		virtual void run();
		
		virtual void stop(bool doJoin = true);
		
		// event methods
		inline void addEvent(Event* e, uint64 time = 0) {
			scheduler->addEvent(e, time);
		}

		inline void addEvent(Event* e, Time& time) {
			scheduler->addEvent(e, time);
		}

		inline void removeEvent(Event* e) {
			scheduler->deleteEvent(e);
		}

		// setters and getters
		inline void setRunning(bool val) {
			doRun = val;
		}
	
		inline bool isRunning() {
			return doRun;
		}
	
		inline void setReady(bool val) {
			serviceReady = val;
		}
	
		inline bool isReady() {
			return serviceReady;
		}

		inline ScheduleManager* getScheduler() {
			return scheduler; 
		}
	
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*SERVICETHREAD_H_*/
