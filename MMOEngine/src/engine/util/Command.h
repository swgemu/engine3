/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef COMMAND_H_
#define COMMAND_H_

namespace engine {
  namespace util {

	class Command {
	public:
		Command() {
		}

		virtual ~Command() {
		}

		virtual void execute() = 0;

		virtual void undo() = 0;
	};

  } // namespace util
} // namespace engine

using namespace engine::util;

#endif /* COMMAND_H_ */
