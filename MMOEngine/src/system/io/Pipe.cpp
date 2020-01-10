/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include "IOException.h"

#include "Pipe.h"

namespace PipeNs {
#ifdef PLATFORM_WIN
	const static Pipe::PipeType NullPipe = reinterpret_cast<Pipe::PipeType>(0);
#else
	const static Pipe::PipeType NullPipe = -1;
#endif
}

using namespace PipeNs;

Pipe::Pipe() {
	for (auto i = 0; i < 2 ; ++i) {
		pipefd[i] = NullPipe;
	}
}

Pipe::~Pipe() {
	close();
}

void Pipe::create(bool autoClose) {
	doAutoClose = autoClose;

#ifndef PLATFORM_WIN
	if (pipe(pipefd) < 0)
		throw IOException("unable to create pipe");

	fileDescriptor = pipefd[0];
#else
	auto res = CreatePipe(&pipefd[0], &pipefd[1], nullptr, 0);

	if (!res) {
		throw IOException("unable to create pipe");
	}
#endif
}

void Pipe::close() {
	for (auto i = 0; i < 2; ++i) {
		closePipe(pipefd[i]);
	}
}

void Pipe::closePipe(PipeType& pipe) {
	if (pipe != NullPipe) {
#ifndef PLATFORM_WIN
		::close(pipe);
#else
		CloseHandle(pipe);
#endif
		pipe = NullPipe;
	}
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

		if (*buf == '\n') {
			++buf;
			break;
		}
	}

	if (len) {
		*buf = 0;
	}

	return count;
}

void Pipe::writeInt(int val) {
	write((char *) &val, sizeof(int));
}

int Pipe::read(char* buf, int len) {
	if (doAutoClose) {
		closePipe(pipefd[1]);
	}

	if (pipefd[0] == NullPipe)
		throw IOException("pipe does not exists on read");

#ifndef PLATFORM_WIN
	int result = ::read(pipefd[0], buf, len);

	if (result < 0)
		throw IOException("unable to read from pipe");

	return result;
#else
	DWORD numberOfBytesRead;

	auto res = ReadFile(pipefd[0], buf, len, &numberOfBytesRead, nullptr);

	if (!res)
		throw IOException("unable to read from pipe");
	
	return numberOfBytesRead;
#endif
}

int Pipe::writeLine(const char* str) {
	int len = strlen(str);

	return write(str, len);
}

int Pipe::write(const String& string) {
	return write(string.toCharArray(), string.length());
}

int Pipe::write(const char* buf, int len) {
	if (doAutoClose) {
		closePipe(pipefd[0]);	
	}

	if (pipefd[1] == NullPipe)
		throw IOException("pipe does not exists on write");

#ifndef PLATFORM_WIN
	int result = ::write(pipefd[1], buf, len);
	if (result < 0)
		throw IOException("unable to write to pipe");

	return result;
#else
	DWORD numberOfBytesWritten;

	auto res = WriteFile(pipefd[1], buf, len, &numberOfBytesWritten, nullptr);

	if (!res)
		throw IOException("unable to write to pipe");

	return numberOfBytesWritten;
#endif
}

void Pipe::redirectFile(PipeType fd) {
#ifndef PLATFORM_WIN
	dup2(pipefd[1], fd);
#else
	E3_ABORT("Pipe::redirectFile file not implemented in windows");
#endif
}
