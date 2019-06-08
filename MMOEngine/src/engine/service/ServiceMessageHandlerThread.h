/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SERVICEMESSAGEHANDLERTHREAD_H_
#define SERVICEMESSAGEHANDLERTHREAD_H_

#include "ServiceThread.h"

#include "ServiceClientMap.h"

#include "ServiceHandler.h"
#include "ServiceFilter.h"

#include "MessageQueue.h"

namespace engine {
  namespace service {
  	
	class ServiceMessageHandlerThread : public ServiceThread {
	protected:
		ServiceClientMap* clients;
	
		Socket* socket;
		int port;
		
		MessageQueue messageQueue;
		
		ServiceHandler* serviceHandler;

		ServiceFilter* serviceFilter;

	public:
		ServiceMessageHandlerThread(const String& s);
		
		virtual ~ServiceMessageHandlerThread();
	
		bool removeConnection(ServiceClient* client);

		void removeConnections();

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

		// setters
		void setHandler(ServiceHandler* handler) {
			serviceHandler = handler;
		}

		void setFilter(ServiceFilter* filter) {
			serviceFilter = filter;
		}
};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*SERVICEMESSAGEHANDLERTHREAD_H_*/
