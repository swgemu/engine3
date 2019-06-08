/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef FILEOUTPUTSTREAM_H_
#define FILEOUTPUTSTREAM_H_

#include "File.h"

#include "OutputStream.h"

#include "FileNotFoundException.h"

namespace sys {
  namespace io {

  	class FileOutputStream : public OutputStream {
  		File* file;

  	public:
  		FileOutputStream(File* file) {
  			file->setBinary();

  			file->setWriteable();

  			FileOutputStream::file = file;
  		}

  		void close() {
  			validateWriteable();

  			file->flush();

  			file->close();
  		}

  		void flush() {
  			validateWriteable();

  			file->flush();
  		}

  		int write(byte* buf, int len) {
  			validateWriteable();

  			return fwrite(buf, 1, len, file->getDescriptor());
  		}

  		int write(byte* buf, uint32 off, int len) {
  			validateWriteable();

  			file->seek(off);

  			return fwrite(buf, 1, len, file->getDescriptor());
  		}

  		int write(char ch) {
			return fwrite(&ch, 1, 1, file->getDescriptor());
		}

  		int write(int val) {
			return fwrite(&val, 1, 4, file->getDescriptor());
		}

  		int write(uint32 val) {
			return fwrite(&val, 1, 4, file->getDescriptor());
		}

  		int write(long val) {
			return fwrite(&val, 1, sizeof(long), file->getDescriptor());
		}

  		int write(int64 val) {
  			return fwrite(&val, 1, 8, file->getDescriptor());
		}

  		int write(uint64 val) {
			return fwrite(&val, 1, 8, file->getDescriptor());
		}

  		int write(float val) {
			return fwrite(&val, 1, sizeof(float), file->getDescriptor());
		}

  		int write(const char* str) {
  			return fwrite(str, 1, strlen(str), file->getDescriptor());
  		}

  		int write(const String& str) {
  			return write((byte*) str.toCharArray(), str.length());
  		}

  		FileOutputStream& operator<< (char ch) {
			write(ch);

			return *this;
		}

  		FileOutputStream& operator<< (int val) {
			write(val);

			return *this;
		}

  		FileOutputStream& operator<< (uint32 val) {
			write(val);

			return *this;
		}

  		FileOutputStream& operator<< (long val) {
			write(val);

			return *this;
		}

  		FileOutputStream& operator<< (int64 val) {
			write(val);

			return *this;
		}

  		FileOutputStream& operator<< (uint64 val) {
			write(val);

			return *this;
		}

  		FileOutputStream& operator<< (float val) {
			write(val);

			return *this;
		}

  		FileOutputStream& operator<< (const char* str) {
			write(str);

			return *this;
		}

  		FileOutputStream& operator<< (const String& str) {
			write(str);

			return *this;
		}

  	protected:
  		void validateWriteable() {
  			if (!file->exists())
  				throw FileNotFoundException(file);
  		}
  	};
  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*FILEOUTPUTSTREAM_H_*/
