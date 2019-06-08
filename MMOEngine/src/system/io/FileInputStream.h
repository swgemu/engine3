/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef FILEINPUTSTREAM_H_
#define FILEINPUTSTREAM_H_

#include "File.h"

#include "InputStream.h"

#include "FileNotFoundException.h"

namespace sys {
  namespace io {

  	class FileInputStream : public InputStream {
  	protected:
  		File* file;

  	public:
  		FileInputStream(File* file) {
  			file->setBinary();

  			file->setReadOnly();

  			FileInputStream::file = file;
  		}

  		void close() {
  			validateReadable();

  			file->close();
  		}

  		int read(byte* buf, int len) {
  			validateReadable();

  			return fread(buf, 1, len, file->getDescriptor());
  		}

  		int read(byte* buf, uint32 off, int len) {
  			validateReadable();

  			file->seek(off);

  			return fread(buf, 1, len, file->getDescriptor());
  		}

  		int skip(int n) {
  			validateReadable();

  			return file->seek(n, SEEK_CUR);
  		}

  	protected:
  		void validateReadable() {
  			if (!file->exists())
  				throw FileNotFoundException(file);
  		}
  	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*FILEINPUTSTREAM_H_*/
