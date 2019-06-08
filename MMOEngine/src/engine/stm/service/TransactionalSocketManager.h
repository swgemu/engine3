/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef TRANSACTIONALSOCKETMANAGER_H_
#define TRANSACTIONALSOCKETMANAGER_H_

#include "engine/util/Command.h"

#include "engine/service/MessageQueue.h"

namespace engine {
  namespace stm {

  	class TransactionalSocketManager : public Command, public Logger {
  		ThreadLocal<MessageQueue*> sentMessages;

  	public:
  		TransactionalSocketManager();

  		void initialize();

  		void sendMessage(Message* message);

  		void execute();

  		void undo();

  	protected:
  		MessageQueue* getLocalMessageQueue();
  	  };

  } // namespace stm
} // namespace engine

using namespace engine::stm;

#endif /* TRANSACTIONALSOCKETMANAGER_H_ */
