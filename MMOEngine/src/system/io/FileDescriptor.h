/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef FILEDESCRIPTOR_H_
#define FILEDESCRIPTOR_H_

#include "system/platform.h"

#include "IOEvent.h"

namespace sys {
  namespace io {

	class FileDescriptor {
	protected:
		int fileDescriptor;

	public:
		FileDescriptor() {
			fileDescriptor = -1;
		}

		virtual void handleEvent(const IOEvent& event) {
		}

		virtual ~FileDescriptor() {

		}

		int getFileDescriptor() {
			return fileDescriptor;
		}
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /* FILEDESCRIPTOR_H_ */
