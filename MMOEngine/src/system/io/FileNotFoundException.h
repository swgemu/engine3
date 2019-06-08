/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef FILENOTFOUNDEXCEPTION_H_
#define FILENOTFOUNDEXCEPTION_H_

#include "IOException.h"

#include "File.h"

namespace sys {
  namespace io {

	class FileNotFoundException : public IOException {
	public:
		FileNotFoundException(File* file) : IOException("FileNotFoundException") {
			message = message + " - file \'" + file->getName() + " not found";
		}
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*FILENOTFOUNDEXCEPTION_H_*/
