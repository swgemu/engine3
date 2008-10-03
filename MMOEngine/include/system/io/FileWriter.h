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
  	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*FILEWRITER_H_*/
