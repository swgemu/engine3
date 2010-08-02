/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DATAGRAMSERVICETHREAD_H_
#define DATAGRAMSERVICETHREAD_H_

#include "AbstractServiceAcceptor.h"

#include "DatagramSession.h"

namespace engine {
  namespace service {
  	
	class DatagramAcceptor : public AbstractServiceAcceptor {
	protected:
		DatagramServerSocket* socket;

	public:
		DatagramAcceptor();
		DatagramAcceptor(const String& s);
		
		virtual ~DatagramAcceptor();
		
		void bind(const SocketAddress& address);
	
		virtual void unbind();

protected:
		// message methods
		void processMessages();

		void removeConnections();
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*DATAGRAMSERVICETHREAD_H_*/
