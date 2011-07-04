/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SERVICEPROCESSTHREAD_H_
#define SERVICEPROCESSTHREAD_H_

#include "ServiceThread.h"
#include "ServiceMessageHandlerThread.h"

namespace engine {
  namespace service {

	class ServiceProcessThread : public ServiceThread {
	protected:
		ServiceMessageHandlerThread* server;
	
	public:
		ServiceProcessThread(const String& s);
	
		virtual ~ServiceProcessThread();
	
		void start(ServiceMessageHandlerThread* serv);
	
		virtual void run();
		
		void stop();
		
		virtual void processMessage(Message* message);
		
		virtual bool handleError(Message* msg, Exception& e);
		
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*SERVICEPROCESSTHREAD_H_*/
