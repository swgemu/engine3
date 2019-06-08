/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef PROCESS_H_
#define PROCESS_H_

#include "system/platform.h"

#include "system/lang/Runnable.h"

#include "system/io/Pipe.h"

namespace sys {
  namespace thread {

  	class Process : public Runnable {
  	protected:
  		pid_t pid;

  		Pipe pipe;

  	public:
  		Process();

  		virtual void initialize();

  		virtual void start();

  		void wait();

  		void signal(int sig);

  		pid_t getPid() const {
  			return pid;
  		}

  		Pipe& getPipe() {
  			return pipe;
  		}
  	};

  } // namespace thread
} // namespace sys

using namespace sys::thread;

#endif /* PROCESS_H_ */
