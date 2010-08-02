/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SERVICEMESSAGEHANDLERTHREAD_H_
#define SERVICEMESSAGEHANDLERTHREAD_H_

#include "ServiceThread.h"

#include "ServiceSession.h"
#include "ServiceSessionMap.h"

#include "ServiceHandler.h"
#include "FilteringServiceHandler.h"

#include "MessageQueue.h"

namespace engine {
  namespace service {
  	
	class AbstractServiceAcceptor : public ServiceThread {
	protected:
		ServiceSession* session;

		ServiceSessionMap* sessions;
		
		MessageQueue messageQueue;
		
		FilteringServiceHandler* handler;

	public:
		AbstractServiceAcceptor(const String& s);
		
		virtual ~AbstractServiceAcceptor();
	
		virtual bool deleteConnection(ServiceSession* client);

		// message functions
		inline void addMessage(Message* msg) {
			messageQueue.push(msg);
		}
		
		inline Message* getMessage() {
			return messageQueue.pop();
		}

		void flushMessages() {
			messageQueue.flush();
		}
		
		// getters
		ServiceSession* getSession() {
			return session;
		}

		MessageQueue* getMessageQueue() {
			return &messageQueue;
		}

		// setters
		void setHandler(ServiceHandler* hand) {
			handler->setHandler(hand);
		}

		void setFilterChain(ServiceFilterChain* chain) {
			handler->setFilterChain(chain);
		}
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*SERVICEMESSAGEHANDLERTHREAD_H_*/
