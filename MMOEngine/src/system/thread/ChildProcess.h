/*
Copyright (C) 2013 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
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
