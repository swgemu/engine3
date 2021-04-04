/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "FileLogWriter.h"

#include <cerrno>
#include <string.h>

namespace FileLogWriterNamespace {
	std::mutex& getMutex() {
		static std::mutex mutex;

		return mutex;
	}

	VectorMap<String, Reference<FileLogWriter*>>& getFileMap() {
		static VectorMap<String, Reference<FileLogWriter*>> map;

		return map;
	}
}

FileLogWriter::FileLogWriter(File* file, bool append, bool rotateAtStart) : FileWriter(file, append) {
	if (rotateAtStart) {
		rotatefile(true);
	}

	currentLoggedBytes.set(file->exists() ? file->size() : 0);
}

FileLogWriter::~FileLogWriter() {
	if (file != nullptr) {
		file->close();
		delete file;
	}
}

void FileLogWriter::close() {
	std::unique_lock<std::mutex> guard(FileLogWriterNamespace::getMutex());

	auto& filemap = FileLogWriterNamespace::getFileMap();
	auto fileName = file->getName();
	auto writer = filemap.get(fileName);

	if (writer != nullptr && writer->getReferenceCount() <= 4) {
		filemap.drop(fileName);
		file->close();
	}
}

Reference<FileLogWriter*> FileLogWriter::getWriter(const String& fileName, bool append, bool rotateAtStart) {
	std::unique_lock<std::mutex> guard(FileLogWriterNamespace::getMutex());

	auto& filemap = FileLogWriterNamespace::getFileMap();
	auto writer = filemap.get(fileName);

	if (writer == nullptr) {
		writer = new FileLogWriter(new File(fileName), append, rotateAtStart);

		if (writer != nullptr) {
			filemap.put(fileName, writer);
		}
	}

	return writer;
}

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

void FileLogWriter::rotatefile(bool force) const {
	if (!force && maxLoggedBytes == 0) {
		return;
	}

	if(file == nullptr || !file->exists() || !rotateMutex.tryLock()) {
		return;
	}

	if (file->size() == 0) {
		rotateMutex.unlock();
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
		char buf[1024];
#ifdef PLATFORM_MAC
		char* msg = buf;
		strerror_r(errno, buf, sizeof(buf));
#else // PLATFORM_MAC
#ifndef PLATFORM_WIN
		char* msg = strerror_r(errno, buf, sizeof(buf));
#else // PLATFORM_WIN
		char* msg = "<unknown on windows>";
#endif // PLATFORM_WIN
#endif // PLATFORM_MAC

		// Need to avoid loop of calling logger that calls us, but we also need someone to notice this issue and resolve it
		StringBuffer errMsg;
		errMsg << "WARNING: Failed to rotate " << fileName << " to " << destFileName << " errno=" << errno << " (" << msg << ")" << endl;
		std::cout << errMsg.toString().toCharArray();
	} else {
#if DEBUG_FILELOGWRITER
		StringBuffer errMsg;
		errMsg << "Rotated " << fileName << " to " << destFileName << " size=" << currentLoggedBytes << endl;
		std::cout << errMsg.toString().toCharArray();
#endif // DEBUG_FILELOGWRITER
		file->reopen();
	}

	// Don't try again until we write another maxLoggedBytes
	currentLoggedBytes.set(0);
	rotateMutex.unlock();
}
