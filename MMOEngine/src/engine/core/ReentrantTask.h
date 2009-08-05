/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef REENTRANTTASK_H_
#define REENTRANTTASK_H_

#include "Task.h"

namespace engine {
  namespace core {

	class ReentrantTask : public Task {
	public:
		ReentrantTask() : Task() {
			setReentrant();
		}

		ReentrantTask(uint64 mtime) : Task(mtime) {
			setReentrant();
		}

		ReentrantTask(Time& time) : Task(time) {
			setReentrant();
		}

	};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /* REENTRANTTASK_H_ */
