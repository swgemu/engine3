/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#pragma once

#include "system/platform.h"
#include "system/lang/String.h"

#include "FileDescriptor.h"

#ifdef PLATFORM_WIN
#include <namedpipeapi.h>
#endif

namespace sys {
  namespace io {

	class Pipe : public FileDescriptor {
	public:
#ifndef PLATFORM_WIN
		using PipeType = int;
#else
		using PipeType = HANDLE;
#endif

	private:
		PipeType pipefd[2];

		bool doAutoClose = false;

	public:
		Pipe();

		~Pipe();

		void create(bool autoClose = true);

		void close();

		int readInt();

		int readLine(char* buf, int len);

		void writeInt(int val);

		int writeLine(const char* str);

		int read(char* buf, int len);

		int write(const char* buf, int len);
		int write(const String& string);

		void redirectFile(PipeType fd);

	private:
		static void closePipe(PipeType& pipe);
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

