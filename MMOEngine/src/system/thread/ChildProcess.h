/*
Copyright (C) 2013 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef CHILDPROCESS_H_
#define CHILDPROCESS_H_

#include "system/platform.h"

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "system/lang/Runnable.h"

namespace sys {
  namespace thread {

  	class ChildProcess :public Runnable {
  		int pid;

  	public:
  		ChildProcess() : pid(0) { }

  		void start() {
  		    pid = fork();
  		    if (!pid) {
  		        //dup2(2,0); // redirect output to stdout

  		    	run();
  		    }
  		}

  		void wait() {
  			if (pid)
  				waitpid(pid,NULL,0);
  		}
  	};

  } // namespace thread
} // namespace sys

using namespace sys::thread;

#endif /* CHILDPROCESS_H_ */
