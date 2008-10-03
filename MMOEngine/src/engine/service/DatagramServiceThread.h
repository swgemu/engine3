/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DATAGRAMSERVICETHREAD_H_
#define DATAGRAMSERVICETHREAD_H_

#include "ServiceMessageHandlerThread.h"

namespace engine {
  namespace service {
  	
	class DatagramServiceThread : public ServiceMessageHandlerThread {
	public:
		DatagramServiceThread(const string& s);
		
		virtual ~DatagramServiceThread();
		
		void start(int p, int mconn = 10);
	
		virtual void stop();

		virtual bool removeConnection(ServiceClient* client);

		void removeConnections();
		
		// message methods
		void receiveMessages();

		virtual void handleMessage(ServiceClient* client, Packet* message) = 0;

		virtual bool handleError(ServiceClient* client, Exception& e);
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*DATAGRAMSERVICETHREAD_H_*/
