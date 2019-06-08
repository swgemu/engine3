/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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
