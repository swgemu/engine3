/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef IOPROCESSOR_H_
#define IOPROCESSOR_H_

#include "system/util/Vector.h"

#include "FileDescriptor.h"

namespace sys {
  namespace io {

	class IOProcessor {
	#ifdef PLATFORM_LINUX
		int epollFileDescritptor;
		int epollQueueLength;
	#else
		Vector<FileDescriptor*> descriptors;
	#endif

		static const int QUEUE_LIMIT = 10000;

	public:
		IOProcessor();

		~IOProcessor();

		void initialize(int queueLength = QUEUE_LIMIT);

		void shutdown();

		int pollEvents(int timeout = 0);

		void addFileDescriptor(FileDescriptor* descriptor, bool edgeTriggered = false);
		void removeFileDescriptor(FileDescriptor* descriptor);
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /* IOPROCESSOR_H_ */
