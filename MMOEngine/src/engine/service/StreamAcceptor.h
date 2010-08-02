/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STREAMSERVICETHREAD_H_
#define STREAMSERVICETHREAD_H_

#include "AbstractServiceAcceptor.h"

namespace engine {
  namespace service {

	class StreamAcceptor : public AbstractServiceAcceptor {
	protected:
		StreamServerSocket* socket;
		int port;

	public:
		StreamAcceptor(const String& s);
		
		virtual ~StreamAcceptor();
		
		void start(int p, int mconn = 10);
		
		virtual void stop();
		
		void acceptConnections(int limit = 15);

		bool closeConnection(ServiceSession* client);

		inline int getServicePort() {
			return port;
		}
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*STREAMSERVICETHREAD_H_*/
