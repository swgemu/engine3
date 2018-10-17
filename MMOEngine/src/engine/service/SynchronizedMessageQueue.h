/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SYNCHRONIZEDMESSAGEQUEUE_H_
#define SYNCHRONIZEDMESSAGEQUEUE_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "Message.h"

namespace engine {
  namespace service {

	class SynchronizedMessageQueue : public ArrayList<Message*> {
		AtomicInteger readIndex, writeIndex;

		bool blocked;

	public:
		SynchronizedMessageQueue(int capacity);

		virtual ~SynchronizedMessageQueue();

		void push(Message* msg);

		Message* pop();

		void flush();

	protected:
		int getBufferSize() {
			return elementCapacity;
		}

	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*SYNCHRONIZEDMESSAGEQUEUE_H_*/
