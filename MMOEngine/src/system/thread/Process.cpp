#include "unistd.h"
#include "errno.h"

#include <sys/wait.h>
#include <signal.h>

#include "Process.h"

Process::Process() : pid(0) {
	initialize();
}

void Process::initialize() {
}

void Process::start() {
	run();
}

void Process::wait() {
	if (pid)
		waitpid(pid, 0, 0);
}

void Process::signal(int sig) {
	kill(pid, sig);
}
