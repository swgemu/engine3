#ifndef FILEWRITER_H_
#define FILEWRITER_H_

#include "File.h"

#include "Writer.h"

#include "FileNotFoundException.h"

namespace sys {
  namespace io {

  	class FileWriter : public Writer {
  		File* file;

  	public:
  		FileWriter(File* file, bool append = false) {
  			file->mkdirs();

  			if (append)
  				file->setAppendable();
  			else
  				file->setWriteable();

  			FileWriter::file = file;
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

  		int write(const char* str, int len) {
  			validateWriteable();

  			return fwrite((byte*) str, 1, len, file->getDescriptor());
  		}

  		int write(const char* str, uint32 off, int len) {
  			validateWriteable();

  			file->seek(off);

  			return fwrite((byte*) str, 1, len, file->getDescriptor());
  		}

  		int write(char ch) {
  			char buf[256];

			buf[0] = ch;

			return write(buf, 1);
		}

  		int write(int val) {
  			char buf[256];

			sprintf(buf, "%i", val);

			return write(buf, strlen(buf));
		}

  		int write(uint32 val) {
  			char buf[256];

			sprintf(buf, "%u", val);

			return write(buf, strlen(buf));
		}

  		int write(long val) {
  			char buf[256];

			sprintf(buf, "%ld", val);

			return write(buf, strlen(buf));
		}

  		int write(int64 val) {
  			char buf[256];

			sprintf(buf, "%ld", (long) val);

			return write(buf, strlen(buf));
		}

  		int write(uint64 val) {
  			char buf[256];

			sprintf(buf, "%lu", (unsigned long) val);

			return write(buf, strlen(buf));
		}

  		int write(float val) {
  			char buf[256];

			sprintf(buf, "%f", val);

			return write(buf, strlen(buf));
		}

  		int write(const char* str) {
  			return write(str, strlen(str));
  		}

  		int write(const String& str) {
  			return write(str.toCharArray(), str.length());
  		}

  		int writeLine(const String& str) {
  			String line = str + "\n";

  			return write(line.toCharArray(), line.length());
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

		FileWriter& operator<< (const StringBuffer& str) {
			write(str.getBuffer(), str.length());

			return *this;
		}

  		inline File* getFile() {
  			return file;
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

#endif /*FILEWRITER_H_*/
