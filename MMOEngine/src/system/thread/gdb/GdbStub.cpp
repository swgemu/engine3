/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include "system/lang/StringBuffer.h"
#include "system/io/StringTokenizer.h"

#include "GdbStub.h"

GdbStub::GdbStub() : logFile(nullptr) {
}

GdbStub::~GdbStub() {
	if (logFile) {
		logFile->close();

		delete logFile;
		logFile = nullptr;
	}
}

void GdbStub::initialize(pid_t pid) {
	processPid = String::valueOf(pid);

	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	File* file= new File("log/crash_" + processPid + "_" + buf);
	logFile = new FileWriter(file);
}

void GdbStub::run() {
	printStackTrace();
	printRegisters();
}

void GdbStub::printStackTrace() {
	pipe.create();

	pid = fork();

	if (!pid) {
		pipe.redirectFile(fileno(stdout));

		execlp("gdb", "gdb",
				"--batch", "-f", "-n",
				"-ex", "set pagination off",
				"-ex", "thread",
				"-ex", "bt full",
				"-pid", processPid.toCharArray(), nullptr);

		E3_ABORT("gdb");
	} else {
		wait();

		logFile->writeLine("Stack Trace:");

		writeOutput();

		logFile->writeLine("");
	}

	pipe.close();
}

void GdbStub::printRegisters() {
	pipe.create();

	pid = fork();

	if (!pid) {
		pipe.redirectFile(fileno(stdout));

		execlp("gdb", "gdb",
				"--batch", "-f", "-n",
				"-ex", "set pagination off",
				"-ex", "info registers",
				"-pid", processPid.toCharArray(), nullptr);

		E3_ABORT("gdb");
	} else {
		wait();

		logFile->writeLine("Registers:");

		writeOutput();

		logFile->writeLine("");
	}

	pipe.close();
}

void GdbStub::printDeadlock() {
	pipe.create();

	pid = fork();

	if (!pid) {
		pipe.redirectFile(fileno(stdout));

		execlp("gdb", "gdb",
				"--batch", "-f", "-n",
				"-ex", "set pagination off",
				"-ex", "info registers",
				"-pid", processPid.toCharArray(), nullptr);

		E3_ABORT("gdb");
	} else {
		wait();

		logFile->writeLine("Registers:");

		writeOutput();

		logFile->writeLine("");
	}

	pipe.close();
}

void GdbStub::printThread(String threadInfo) {
	if (threadInfo.contains("pthread_cond_timedwait") || threadInfo.contains("pthread_cond_wait"))
		return;

	StringTokenizer tokenizer(threadInfo);

	int threadID = tokenizer.getIntToken();

	String threadString;
	tokenizer.getStringToken(threadString);
	if (threadString.compareTo("Thread") != 0)
		return;

	pipe.create();

	pid = fork();

	if (pid == 0) {
		pipe.redirectFile(fileno(stdout));

		char threadCommand[100];
		sprintf(threadCommand, "thread %i", threadID);

		execlp("gdb", "gdb",
				"--batch", "-f", "-n",
				"-ex", "set pagination off",
				"-ex", threadCommand,
				//"-ex", "thread 1",
				"-ex", "bt full",
				"-pid", processPid.toCharArray(), nullptr);

		E3_ABORT("gdb");
	} else {
		wait();

		logFile->writeLine("Thread " + String::valueOf(threadID) + ":");

		writeOutput();

		logFile->writeLine("");
	}

	pipe.close();
}

void GdbStub::getThreads(Vector<String>& threads) {
	pipe.create();

	pid = fork();

	if (!pid) {
		pipe.redirectFile(fileno(stdout));

		execlp("gdb", "gdb",
				"--batch", "-f", "-n",
				"-ex", "set pagination off",
				"-ex", "info threads",
				"-pid", processPid.toCharArray(), nullptr);

		E3_ABORT("gdb");
	} else {
		wait();

		parseOutput(threads);
	}

	pipe.close();
}

void GdbStub::writeOutput() {
	char line[4096];

	while (pipe.readLine(line, 4096) > 0) {
		String str(line);
		if (str.beginsWith("[New Thread") || str.beginsWith("[Thread debugging"))
			continue;

		logFile->write(line);
	}
}

void GdbStub::parseOutput(Vector<String>& lines) {
	char line[4096];

	while (pipe.readLine(line, 4096) > 0) {
		String str(line);
		if (str.beginsWith("[New Thread") || str.beginsWith("[Thread debugging"))
			continue;

		lines.add(str);
	}
}
