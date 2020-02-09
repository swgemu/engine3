/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "FileLogWriter.h"

int FileLogWriter::write(const char* str, int len) {
	validateWriteable();

	if (doSynchronized) {
		writeMutex.lock();
	}

	auto result = fwrite(str, 1, len, file->getDescriptor());

	currentLoggedBytes.add(len);

	if (currentLoggedBytes >= maxLoggedBytes) {
		rotatefile();
	}

	if (doSynchronized) {
		writeMutex.unlock();
	}

	return result;
}

void FileLogWriter::rotatefile() const {
	if (file == nullptr || maxLoggedBytes == 0 || !rotateMutex.tryLock()) {
		return;
	}

	Time now;
	String fileName = file->getName();
	String dir = file->getDirName();
	String baseFilename = file->getBaseName();
	String ext;

	int i = baseFilename.lastIndexOf('.');

	if (i > 0) {
		ext = baseFilename.subString(i);
		baseFilename = baseFilename.subString(0, i);
	}

	StringBuffer buf;
	buf << dir << File::directorySeparator() << rotatePrefix;

	File::mkpath(buf.toString());

	buf << baseFilename << "-" << now.getMiliTime() << ext;
	String destFileName = buf.toString();

	int err = std::rename(fileName.toCharArray(), destFileName.toCharArray());

	if (err != 0) {
		// Need to avoid loop of calling logger that calls us, but we also need someone to notice this issue and resolve it
		System::err << "WARNING: Failed to rotate " << fileName << " to " << destFileName << " err=" << err << endl;
	} else {
#if DEBUG_FILELOGWRITER
		System::out << "Rotated " << fileName << " to " << destFileName << " size=" << currentLoggedBytes << endl;
#endif // DEBUG_FILELOGWRITER
		file->reopen();
	}

	// Don't try again until we write another maxLoggedBytes
	currentLoggedBytes.set(0);
	rotateMutex.unlock();
}
