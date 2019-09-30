/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#pragma once

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
		~File();

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
		inline bool exists() const {
			return fileDescriptor != nullptr;
		}

		inline const String& getName() const {
			return name;
		}

		// setters
  		inline void setBinary() {
  			mode = BINARY_MODE;
  		}

		inline void setText() {
			mode = TEXT_MODE;
		}

	protected:
		bool open(int access);

		static bool doMkdir(const char* path, int mode);

		static String getModeString(int mode, int access);
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

