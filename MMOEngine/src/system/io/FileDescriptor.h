/*
Copyright (C) 2013 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef FILEDESCRIPTOR_H_
#define FILEDESCRIPTOR_H_

#include "system/platform.h"

namespace sys {
  namespace io {

  	class IOHandler;

	class FileDescriptor {
	protected:
		int fileDescriptor;

		IOHandler* handler;

	public:
		FileDescriptor() : fileDescriptor(-1), handler(NULL) {
		}

		virtual ~FileDescriptor() {
		}

		void setBlocking(bool b);

		int getFileDescriptor() {
			return fileDescriptor;
		}

		IOHandler* getHandler() const {
			return handler;
		}

		void setHandler(IOHandler* hand) {
			handler = hand;
		}
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /* FILEDESCRIPTOR_H_ */
