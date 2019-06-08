/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "gdb/GdbStub.h"

#include "system/io/IOProcessor.h"
#include "system/io/IOHandler.h"

#include "ChildProcess.h"

class ChildHandler : public IOHandler
{
	static ChildProcess* process;

	bool crashed;

public:
	enum {SEGFAULT, PING};

	ChildHandler(ChildProcess* proc) : crashed(false) {
		process = proc;
	}

	void install() {
		struct sigaction act;
		act.sa_sigaction = &ChildHandler::handleSignal;

		sigemptyset(&act.sa_mask);
		act.sa_flags = SA_SIGINFO;

		if (sigaction(SIGSEGV, &act, 0) < 0) {
			printf("ChildHandler::ChildHandler(): failed to register sigsegv, errno: %d\n", errno);
			assert(0);
		}
	}

	static void handleSignal(int sig, siginfo_t* info, void*) {
		pid_t pid = info->si_pid;

		/*printf("ChildHandler::handleSignal(): catcher pid %d, signal %d, code: %d, status: %d, pid: %d\n",
				getpid(), sig, info->si_code, info->si_status, pid);*/

		assert(sig == SIGSEGV);

		Pipe& pipe = process->getPipe();
		pipe.writeInt(SEGFAULT);

		sigpause(SIGINT);
	}

	void pollEvents() {
	#ifdef PLATFORM_LINUX
		Pipe& pipe = process->getPipe();

		IOProcessor processor;

		try {
			pipe.setHandler(this);

			processor.initialize(1);
			processor.addFileDescriptor(&pipe, false);
		} catch (const IOException& e) {
			e.printStackTrace();
			throw Exception(e.getMessage());
		}

		while (processor.pollEvents(60000) > 0) {
			if (crashed)
				return;
		}

		handleDeadlock();
	#endif
	}

	void handleInput(FileDescriptor* descriptor) {
		Pipe* pipe = (Pipe*) descriptor;

		int event = pipe->readInt();
		switch  (event) {
		case ChildHandler::SEGFAULT:
			handleCrash();

			crashed = true;

			break;
		case ChildHandler::PING:
			break;
		default:
			assert(0);
		}
	}

	static void sendPing() {
		Pipe& pipe = process->getPipe();
		pipe.writeInt(PING);

		//printf("ChildHandler::sendPing(): sending ping from %u\n", process->getPid());
	}

	void handleCrash() {
		process->signal(SIGSTOP);

		//printf("ChildHandler::handleCrash(): child process %u crashed\n", process->getPid());

		process->printCrash();

		process->handleCrash();

		process->signal(SIGCONT);

		process->signal(SIGINT);
		process->wait();
	}

	void handleDeadlock() {
		process->signal(SIGSTOP);

		//printf("ChildHandler::handleDeadlock(): ping timeout on child process %u\n", process->getPid());

		process->printDeadlock();

		process->handleDeadlock();

		process->signal(SIGKILL);

		process->wait();
	}

	static void checkProcess() {
		if (!process->isDeadlocked())
			sendPing();
	}
};

ChildProcess* ChildHandler::process = 0;

class WatchDogThread: public Thread {
public:
	void run() {
		while (true) {
			ChildHandler::checkProcess();

			Thread::sleep(15000);
		}
	}
};

ChildProcess::ChildProcess() {
}

void ChildProcess::initialize() {
	pipe.create();
}

void ChildProcess::start() {
	initialize();

	ChildHandler handler(this);

	pid = fork();

	if (!pid) {
		//printf("ChildProcess::start(): started child process %u\n", pid);

		handler.install();

		Thread* watchdog = new WatchDogThread();
		watchdog->start();

		run();
	} else {
		handler.pollEvents();
	}
}

void ChildProcess::printCrash() {
	GdbStub gdb;
	gdb.initialize(pid);

	gdb.printStackTrace();
	gdb.printRegisters();
}

void ChildProcess::printDeadlock() {
	GdbStub gdb;
	gdb.initialize(pid);

	gdb.printStackTrace();
	gdb.printRegisters();

	Vector<String> threads;
	gdb.getThreads(threads);

	for (int i = 0; i < threads.size(); ++i) {
		gdb.printThread(threads.get(i));
	}
}
