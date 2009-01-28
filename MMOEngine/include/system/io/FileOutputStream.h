#ifndef FILEOUTPUTSTREAM_H_
#define FILEOUTPUTSTREAM_H_

#include "File.h"

#include "OutputStream.h"

#include "FileNotFoundException.h"

namespace sys {
  namespace io {

  	class FileOutputStream : public OutputStream {
  		File* file;

  		char buf[256];

  	public:
  		FileOutputStream(File* file) {
  			file->setBinary();

  			file->setWriteable();

  			if (!file->exists())
  				throw FileNotFoundException(file);

  			FileOutputStream::file = file;
  		}

  		void close() {
  			file->flush();

  			file->close();
  		}

  		void flush() {
  			file->flush();
  		}

  		int write(byte* buf, int len) {
  			return fwrite(buf, 1, len, file->getDescriptor());
  		}

  		int write(byte* buf, uint32 off, int len) {
  			file->seek(off);

  			return fwrite(buf, 1, len, file->getDescriptor());
  		}

  		int write(char ch) {
			buf[0] = (byte) ch;

			return write((byte*) buf, 1);
		}

  		int write(int val) {
			*((int*) buf) = val;

			return write((byte*) buf, 4);
		}

  		int write(uint32 val) {
  			*((uint32*) buf) = val;

			return write((byte*) buf, 4);
		}

  		int write(long val) {
  			*((long*) buf) = val;

			return write((byte*) buf, sizeof(long));
		}

  		int write(int64 val) {
  			*((int64*) buf) = val;

			return write((byte*) buf, 8);
		}

  		int write(uint64 val) {
  			*((uint64*) buf) = val;

			return write((byte*) buf, 8);
		}

  		int write(float val) {
  			*((float*) buf) = val;

			return write((byte*) buf, sizeof(float));
		}

  		int write(const char* str) {
  			return write((byte*) str, strlen(str));
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
  	};
  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*FILEOUTPUTSTREAM_H_*/
