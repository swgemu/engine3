#ifndef FILE_H_
#define FILE_H_

#include "system/platform.h"

#include "system/lang/String.h"

namespace sys {
  namespace io {

	class File {
	protected:
		FILE* fileDescriptor;

		String name;

		int mode;

		int access;

		int permissions;

		static const int TEXT_MODE = 1;

		static const int BINARY_MODE = 2;

		static const int READONLY_ACCESS = 1;

		static const int WRITEABLE_ACCESS = 2;

		static const int APPENDABLE_ACCESS = 3;

	public:
		File(const String& pathname);

		bool close();

		void flush();

		bool deleteFile();

		static int deleteFile(const char* name);

		FILE* getDescriptor() const;

		bool mkdir();
		bool mkdirs();

		int seek(long offset, int origin = SEEK_SET);
		long size();

  		bool setReadOnly();

  		bool setWriteable();

  		bool setAppendable();

		// getters
		inline bool exists() {
			return fileDescriptor != nullptr;
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

		static bool doMkdir(const char* path, int mode);

		String getModeString(int mode, int access);
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*FILE_H_*/
