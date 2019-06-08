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

void Pipe::create() {
	if (pipe(pipefd) < 0)
		throw IOException("unable to create pipe");

	fileDescriptor = pipefd[0];
}

void Pipe::close() {
	::close(pipefd[0]);
	::close(pipefd[1]);
}

int Pipe::readInt() {
	int value;
	read((char*) &value, sizeof(int));
	return value;
}

int Pipe::readLine(char* buf, int len) {
	int count = 0;
	for (; count < len; ++count, ++buf) {
		if (read(buf, 1) == 0)
			break;

		//printf("red %02i (%p)\n", (int) *buf, buf);

		if (*buf == '\n') {
			++buf;
			break;
		}
	}

	*buf = 0;

	return count;
}

void Pipe::writeInt(int val) {
	write((char *) &val, sizeof(int));
}

int Pipe::read(char* buf, int len) {
	if (pipefd[1] != -1) {
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
}

int Pipe::writeLine(const char* str) {
	int len = strlen(str);

	return write(str, len);
}

int Pipe::write(const char* buf, int len) {
	if (pipefd[0] != -1) {
		::close(pipefd[0]);
		pipefd[0] = -1;
	}

	if (pipefd[1] == -1)
		throw IOException("pipe does not exists on write");

	int result = ::write(pipefd[1], buf, len);
	if (result < 0)
		throw IOException("unable to write to pipe");

	return result;
}

void Pipe::redirectFile(int fd) {
	dup2(pipefd[1], fd);
}
