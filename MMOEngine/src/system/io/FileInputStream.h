#ifndef FILEINPUTSTREAM_H_
#define FILEINPUTSTREAM_H_

#include "File.h"

#include "InputStream.h"

#include "FileNotFoundException.h"

namespace sys {
  namespace io {

  	class FileInputStream : public InputStream {
  		File* file;

  	public:
  		FileInputStream(File* file) {
  			file->setBinary();

  			file->setReadOnly();

  			if (!file->exists())
  				throw FileNotFoundException(file);

  			FileInputStream::file = file;
  		}

  		void close() {
  			file->close();
  		}

  		int read(byte* buf, int len) {
  			return fread(buf, 1, len, file->getDescriptor());
  		}

  		int read(byte* buf, uint32 off, int len) {
  			file->seek(off);

  			return fread(buf, 1, len, file->getDescriptor());
  		}

  		int skip(int n) {
  			return file->seek(n, SEEK_CUR);
  		}
  	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*FILEINPUTSTREAM_H_*/
