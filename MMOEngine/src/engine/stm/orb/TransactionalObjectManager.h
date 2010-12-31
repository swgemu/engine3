/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TRANSACTIONALOBJECTMANAGER_H_
#define TRANSACTIONALOBJECTMANAGER_H_

#include "engine/util/Command.h"

namespace engine {
  namespace stm {

  	class TransactionalObjectManager : public Command {
  	public:
  		TransactionalObjectManager();

  		void initialize();

  		void execute();

  		void undo();

  	};

  } // namespace stm
} // namespace engine

using namespace engine::stm;

#endif /* TRANSACTIONALOBJECTMANAGER_H_ */
