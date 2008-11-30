/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SERVICEMESSAGEHANDLERTHREAD_H_
#define SERVICEMESSAGEHANDLERTHREAD_H_

#include "ServiceThread.h"

#include "ServiceClientMap.h"

#include "MessageQueue.h"

namespace engine {
  namespace service {
  	
	class ServiceMessageHandlerThread : public ServiceThread {
	protected:
		ServiceClientMap* clients;
	
		Socket* socket;
		int port;
		
		MessageQueue messageQueue;
		
	public:
		ServiceMessageHandlerThread(const String& s);
		
		virtual ~ServiceMessageHandlerThread();
	
		virtual ServiceClient* createConnection(Socket* sock, SocketAddress& addr) = 0;
		
		virtual bool deleteConnection(ServiceClient* client);

		// message functions
		inline void addMessage(Message* msg) {
			messageQueue.push(msg);
		}
		
		inline Message* getMessage() {
			return messageQueue.pop();
		}

		inline void flushMessages() {
			messageQueue.flush();
		}
		
		// getters
		inline MessageQueue* getMessageQueue() {
			return &messageQueue;
		}

		inline int getServicePort() {
			return port;
		}
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*SERVICEMESSAGEHANDLERTHREAD_H_*/
