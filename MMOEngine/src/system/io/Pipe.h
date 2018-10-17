#ifndef PIPE_H_
#define PIPE_H_

#include "system/platform.h"

#include "FileDescriptor.h"

namespace sys {
  namespace io {

	class Pipe : public FileDescriptor {
		int pipefd[2];

	public:
		Pipe();

		~Pipe();

		void create();

		void close();

		int readInt();

		int readLine(char* buf, int len);

		void writeInt(int val);

		int writeLine(const char* str);

		int read(char* buf, int len);

		int write(const char* buf, int len);

		void redirectFile(int fd);
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*PIPE_H_*/
