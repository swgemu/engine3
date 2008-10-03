#ifndef FILENOTFOUNDEXCEPTION_H_
#define FILENOTFOUNDEXCEPTION_H_

#include "../lang/Exception.h"

#include "File.h"

namespace sys {
  namespace io {
  
	class FileNotFoundException : public Exception {
	public:
		FileNotFoundException(File* file) : Exception("FileNotFoundException") {
			message = message + " - file \'" + file->getName() + " not found";
		}
	
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*FILENOTFOUNDEXCEPTION_H_*/
