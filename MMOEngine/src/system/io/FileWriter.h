#ifndef FILEWRITER_H_
#define FILEWRITER_H_

#include "File.h"

#include "Writer.h"

#include "FileNotFoundException.h"

namespace sys {
  namespace io {

  	class FileWriter : public Writer {
  		File* file;

  		char buf[256];

  	public:
  		FileWriter(File* file) {
  			file->setWriteable();

  			if (!file->exists())
  				throw FileNotFoundException(file);

  			FileWriter::file = file;
  		}

  		void close() {
  			file->flush();

  			file->close();
  		}

  		void flush() {
  			file->flush();
  		}

  		int write(const char* str, int len) {
  			return fwrite((byte*) str, 1, len, file->getDescriptor());
  		}

  		int write(const char* str, uint32 off, int len) {
  			file->seek(off);

  			return fwrite((byte*) str, 1, len, file->getDescriptor());
  		}

  		int write(char ch) {
			buf[0] = ch;

			return write(buf, 1);
		}

  		int write(int val) {
			sprintf(buf, "%i", val);

			return write(buf, strlen(buf));
		}

  		int write(uint32 val) {
			sprintf(buf, "%u", val);

			return write(buf, strlen(buf));
		}

  		int write(long val) {
			sprintf(buf, "%ld", val);

			return write(buf, strlen(buf));
		}

  		int write(int64 val) {
			sprintf(buf, "%ld", (long) val);

			return write(buf, strlen(buf));
		}

  		int write(uint64 val) {
			sprintf(buf, "%lu", (unsigned long) val);

			return write(buf, strlen(buf));
		}

  		int write(float val) {
			sprintf(buf, "%f", val);

			return write(buf, strlen(buf));
		}

  		int write(const char* str) {
  			return write(str, strlen(str));
  		}

  		int write(const String& str) {
  			return write(str.toCharArray(), str.length());
  		}

  		FileWriter& operator<< (char ch) {
			write(ch);

			return *this;
		}

  		FileWriter& operator<< (int val) {
			write(val);

			return *this;
		}

  		FileWriter& operator<< (uint32 val) {
			write(val);

			return *this;
		}

  		FileWriter& operator<< (long val) {
			write(val);

			return *this;
		}

  		FileWriter& operator<< (int64 val) {
			write(val);

			return *this;
		}

  		FileWriter& operator<< (uint64 val) {
			write(val);

			return *this;
		}

  		FileWriter& operator<< (float val) {
			write(val);

			return *this;
		}

  		FileWriter& operator<< (const char* str) {
			write(str);

			return *this;
		}

  		FileWriter& operator<< (const String& str) {
			write(str);

			return *this;
		}
  	};
  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*FILEWRITER_H_*/
