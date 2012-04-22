/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef IOPROCESSOR_H_
#define IOPROCESSOR_H_

#include "FileDescriptor.h"

namespace sys {
  namespace io {

	class IOProcessor {
		int epollFileDescritptor;

		int epollQueueLength;

		static const int EPOLL_QUEUE_LENGTH = 10000;

	public:
		IOProcessor();

		~IOProcessor();

		void initialize(int queueLength = EPOLL_QUEUE_LENGTH);

		void shutdown();

		void pollEvents(int timeout = 0);

		void addFileDescriptor(FileDescriptor* descriptor);
		void removeFileDescriptor(FileDescriptor* descriptor);
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /* IOPROCESSOR_H_ */
