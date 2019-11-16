/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

//#define TRACE_MESSAGES

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "Message.h"

namespace engine {
  namespace service {

	class MessageQueue : public Vector<Message*>, public Condition, public Logger {
		Mutex* condMutex;

		bool blocked;
		//bool waitingForMessage = false;

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
