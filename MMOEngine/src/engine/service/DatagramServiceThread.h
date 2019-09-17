/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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

		void processMessage(Packet* packet, const SocketAddress& addr);

		void receiveMessage(Packet* packet, SocketAddress& addr);

		//friend class MessageReceiverTask;
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*DATAGRAMSERVICETHREAD_H_*/
