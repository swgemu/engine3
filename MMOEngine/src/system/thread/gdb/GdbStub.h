/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef GDBSTUB_H_
#define GDBSTUB_H_

#include "system/thread/Process.h"

#include "system/lang/String.h"

#include "system/io/FileWriter.h"
#include "system/io/Pipe.h"

#include "system/util/Vector.h"

namespace sys {
  namespace thread {

  	class GdbStub : public Process {
		String processPid;
		Vector<String> threads;

		Pipe pipe;

		FileWriter* logFile;

  	public:
  		GdbStub();

  		~GdbStub();

  		void initialize(pid_t pid);

  		void run();

  		void printStackTrace();
  		void printRegisters();

  		void printDeadlock();

  		void printThread(String threadInfo);
  		void getThreads(Vector<String>& threads);

  	private:
  		void writeOutput();

  		void parseOutput(Vector<String>& lines);
  	};

  } // namespace thread
} // namespace sys

using namespace sys::thread;

#endif /* CHILDPROCESS_H_ */
