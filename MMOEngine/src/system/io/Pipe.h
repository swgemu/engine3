/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#pragma once

#include "system/platform.h"
#include "system/lang/String.h"

#include "FileDescriptor.h"

namespace sys {
  namespace io {

	class Pipe : public FileDescriptor {
		int pipefd[2];
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

		void redirectFile(int fd);
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

