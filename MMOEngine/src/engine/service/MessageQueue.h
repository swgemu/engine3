/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

//#define TRACE_MESSAGES

#include "../../system/lang.h"

#include "../log/Logger.h"

#include "Message.h"

namespace engine {
  namespace service {

	class MessageQueue : public Vector<Message*>, public Condition, public Logger {
		Mutex* condMutex;

		bool blocked;
		bool waitingForMessage;

	public:
		MessageQueue();

		virtual ~MessageQueue();

		void push(Message* msg);

		Message* pop();

		void flush();
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*MESSAGEQUEUE_H_*/
