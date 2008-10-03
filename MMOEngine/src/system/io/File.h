#ifndef FILE_H_
#define FILE_H_

#include "../platform.h"

namespace sys {
  namespace io {

	class File {
	protected:
		FILE* fileDescriptor;
		
		string name;
		
		int mode;
	
		static const int READONLY = 1;
		
		static const int WRITEABLE = 2;
		
	public:
		File(const string& pathname);

		bool close();

		void flush();

		FILE* getDescriptor() const;

  		int seek(uint32 offset, int origin = SEEK_SET);

  		bool setReadOnly();

  		bool setWriteable();

		// getters
		inline bool exists() {
			return fileDescriptor != NULL;
		}
		
		inline const string& getName() {
			return name;
		}

	protected:
		bool open(int mode);
		
		char* getModeString(int mode);
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*FILE_H_*/
