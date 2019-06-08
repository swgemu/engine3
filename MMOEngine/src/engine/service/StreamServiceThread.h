/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef STREAMSERVICETHREAD_H_
#define STREAMSERVICETHREAD_H_

#include "ServiceMessageHandlerThread.h"

namespace engine {
  namespace service {

	class StreamServiceThread : public ServiceMessageHandlerThread {
	public:
		StreamServiceThread(const String& s);
		
		virtual ~StreamServiceThread();
		
		void start(int p, int mconn = 10);
		
		virtual void stop();
		
		void acceptConnections(int limit = 15);

		bool closeConnection(ServiceClient* client);

	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*STREAMSERVICETHREAD_H_*/
