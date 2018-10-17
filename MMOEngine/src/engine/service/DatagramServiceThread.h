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
		DatagramServiceThread();
		DatagramServiceThread(const String& s);
		
		virtual ~DatagramServiceThread();
		
		void start(int p, int mconn = 10);
	
		void run();

		void stop();

	//protected:
		// message methods
		void receiveMessages();

		void processMessage(Packet* packet, SocketAddress& addr);

		void receiveMessage(Packet* packet, SocketAddress& addr);

		//friend class MessageReceiverTask;
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*DATAGRAMSERVICETHREAD_H_*/
