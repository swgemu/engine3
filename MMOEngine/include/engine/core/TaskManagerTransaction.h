/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TASKMANAGERTRANSACTION_H_
#define TASKMANAGERTRANSACTION_H_

#include "engine/util/Command.h"

namespace engine {
  namespace core {

  	  class TaskManagerTransaction : public Command {
  	  public:
  		void execute();

  		void undo();
  	  };

  } // namespace service
} // namespace engine

using namespace engine::core;

#endif /* TASKMANAGERTRANSACTION_H_ */
