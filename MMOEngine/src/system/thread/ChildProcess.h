/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef CHILDPROCESS_H_
#define CHILDPROCESS_H_

#include "Process.h"

namespace sys {
  namespace thread {

  	class ChildProcess : public Process {
  	public:
  		ChildProcess();

  		void initialize();

  		void start();

  		virtual bool isDeadlocked() {
  			return false;
  		}

  		virtual void handleCrash() {
  		}

  		virtual void handleDeadlock() {
  		}

  		void printCrash();
  		void printDeadlock();
  	};

  } // namespace thread
} // namespace sys

using namespace sys::thread;

#endif /* CHILDPROCESS_H_ */
