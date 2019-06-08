/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef FILEREADER_H_
#define FILEREADER_H_

#include "File.h"

#include "Reader.h"

#include "FileNotFoundException.h"

namespace sys {
  namespace io {

  	class FileReader : public Reader {
  		File* file;

  	public:
  		FileReader(File* file) {
  			file->setReadOnly();

  			FileReader::file = file;
  		}

  		void close() {
  			validateReadable();

  			file->close();
  		}

  		int read(char* buf, int len) {
  			validateReadable();

  			return fread(buf, 1, len, file->getDescriptor());
  		}

  		int read(char* buf, uint32 off, int len) {
  			validateReadable();

  			file->seek(off);

  			return fread(buf, 1, len, file->getDescriptor());
  		}

  		bool readLine(String& line) {
  			validateReadable();

  			char buf[4096];

  			if (fgets(buf, 4096, file->getDescriptor()) != nullptr) {
  				line = buf;

  				return true;
  			} else
  				return false;
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

#endif /*FILEREADER_H_*/
