/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include "IOException.h"

#include "Pipe.h"


Pipe::Pipe() {
	pipefd[0] = -1;
	pipefd[1] = -1;
}

Pipe::~Pipe() {
	close();
}

void Pipe::create(bool autoClose) {
#ifndef PLATFORM_WIN
	doAutoClose = autoClose;

	if (pipe(pipefd) < 0)
		throw IOException("unable to create pipe");

	fileDescriptor = pipefd[0];
#endif
}

void Pipe::close() {
#ifndef PLATFORM_WIN
	if (pipefd[0] != -1) {
		::close(pipefd[0]);
		pipefd[0] = -1;
	}

	if (pipefd[1] != -1) {
		::close(pipefd[1]);
		pipefd[1] = -1;
	}
#endif
}

int Pipe::readInt() {
	int value;
	read((char*) &value, sizeof(int));
	return value;
}

int Pipe::readLine(char* buf, int len) {
	int count = 0;

#ifndef PLATFORM_WIN
	for (; count < len; ++count, ++buf) {
		if (read(buf, 1) == 0)
			break;

		//printf("red %02i (%p)\n", (int) *buf, buf);

		if (*buf == '\n') {
			++buf;
			break;
		}
	}

	if (len) {
		*buf = 0;
	}
#endif

	return count;
}

void Pipe::writeInt(int val) {
	write((char *) &val, sizeof(int));
}

int Pipe::read(char* buf, int len) {
#ifndef PLATFORM_WIN
	if (doAutoClose && pipefd[1] != -1) {
		::close(pipefd[1]);
		pipefd[1] = -1;
	}

	if (pipefd[0] == -1)
		throw IOException("pipe does not exists on read");

	int result = ::read(pipefd[0], buf, len);
	if (result < 0)
		throw IOException("unable to read from pipe");

	//printf("read %i bytes from pipe\n", result);

	return result;
#else
	return 0;
#endif;
}

int Pipe::writeLine(const char* str) {
	int len = strlen(str);

	return write(str, len);
}

int Pipe::write(const String& string) {
	return write(string.toCharArray(), string.length());
}

int Pipe::write(const char* buf, int len) {
#ifndef PLATFORM_WIN
	if (doAutoClose && pipefd[0] != -1) {
		::close(pipefd[0]);
		pipefd[0] = -1;
	}

	if (pipefd[1] == -1)
		throw IOException("pipe does not exists on write");

	int result = ::write(pipefd[1], buf, len);
	if (result < 0)
		throw IOException("unable to write to pipe");

	return result;
#else
	return 0;
#endif
}

void Pipe::redirectFile(int fd) {
#ifndef PLATFORM_WIN
	dup2(pipefd[1], fd);
#endif
}
