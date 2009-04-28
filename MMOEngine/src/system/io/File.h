#ifndef FILE_H_
#define FILE_H_

#include "../platform.h"

#include "../lang/types/String.h"

namespace sys {
  namespace io {

	class File {
	protected:
		FILE* fileDescriptor;

		String name;

		int mode;

		int access;

		static const int TEXT_MODE = 1;

		static const int BINARY_MODE = 2;

		static const int READONLY_ACCESS = 1;

		static const int WRITEABLE_ACCESS = 2;

	public:
		File(const String& pathname);

		bool close();

		void flush();

		bool deleteFile();

		FILE* getDescriptor() const;

  		int seek(uint32 offset, int origin = SEEK_SET);

  		bool setReadOnly();

  		bool setWriteable();

		// getters
		inline bool exists() {
			return fileDescriptor != NULL;
		}

		inline const String& getName() {
			return name;
		}

		// setters
  		inline void setBinary() {
  			mode = BINARY_MODE;
  		}

	protected:
		bool open(int access);

		String getModeString(int mode, int access);
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*FILE_H_*/
