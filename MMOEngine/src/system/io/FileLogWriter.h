/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "FileWriter.h"
#include "system/util/VectorMap.h"
#include "system/lang/ref/Reference.h"
#include <mutex>

namespace sys {
namespace io {
	class FileLogWriter : public FileWriter {
	private:
		bool doSynchronized = false;
		mutable Mutex writeMutex;
		mutable Mutex rotateMutex;
		mutable AtomicInteger currentLoggedBytes;
		uint32 maxLoggedBytes = 0;
		String rotatePrefix = "zArchive/"; // Default to {dir}/zArchive/{filename}

		FileLogWriter(File* file, bool append = false, bool rotateAtStart = false);
		~FileLogWriter();

	public:
		static Reference<FileLogWriter*> getWriter(const String& fileName, bool append = false, bool rotateAtStart = false);

		inline void setSynchronized(bool synchronized) {
			doSynchronized = synchronized;
		}

		inline void setRotateSizeMB(uint32 maxSizeMB) {
			maxLoggedBytes = maxSizeMB * 1024 * 1024;
		}

		inline void setRotatePrefix(String prefix) {
			rotatePrefix = prefix; // Setup rotate to {dir}{prefix}{filename}
		}

		void close() override;
		int write(const char* str, int len) override;
		void rotatefile(bool force = false) const;
	};
} // namespace io
} // namespace sys

using namespace sys::io;
