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

  		int write(byte* buf, int len) {
  			return fwrite(buf, 1, len, file->getDescriptor());
  		}

  		int write(byte* buf, uint32 off, int len) {
  			file->seek(off);

  			return fwrite(buf, 1, len, file->getDescriptor());
  		}

  		int writeLine(const char* str) {
  			return fwrite((byte*) str, 1, strlen(str), file->getDescriptor());
  		}

  		int writeLine(const String& str) {
  			return fwrite((byte*) str.toCharArray(), 1, str.length(), file->getDescriptor());
  		}

		FileWriter& operator<< (char ch) {
			buf[0] = (byte) ch;

			write((byte*) buf, 1);

			return *this;
		}

		FileWriter& operator<< (int val) {
			sprintf(buf, "%i", val);

			write((byte*) buf, strlen(buf));

			return *this;
		}

		FileWriter& operator<< (uint32 val) {
			sprintf(buf, "%u", val);

			write((byte*) buf, strlen(buf));

			return *this;
		}

		FileWriter& operator<< (long val) {
			sprintf(buf, "%ld", val);

			write((byte*) buf, strlen(buf));

			return *this;
		}

		FileWriter& operator<< (int64 val) {
			sprintf(buf, "%ld", (long) val);

			write((byte*) buf, strlen(buf));

			return *this;
		}

		FileWriter& operator<< (uint64 val) {
			sprintf(buf, "%lu", (unsigned long) val);

			write((byte*) buf, strlen(buf));

			return *this;
		}

		FileWriter& operator<< (float val) {
			sprintf(buf, "%f", val);

			write((byte*) buf, strlen(buf));

			return *this;
		}

		FileWriter& operator<< (const char* str) {
			writeLine(str);

			return *this;
		}

		FileWriter& operator<< (const String& str) {
			writeLine(str);

			return *this;
}
  	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*FILEWRITER_H_*/
