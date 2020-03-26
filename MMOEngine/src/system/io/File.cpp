/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include <sys/stat.h>

#include "system/platform.h"

#ifdef PLATFORM_WIN
#include <direct.h>
#endif

#include <cerrno>

#include <libgen.h>

#include "File.h"

#include "system/thread/Mutex.h"
#include "system/thread/Locker.h"

#include <mutex>

File::File(const String& pathname) {
	name = pathname;

	fileDescriptor = nullptr;

	mode = TEXT_MODE;
	access = READONLY_ACCESS;
	permissions = 0444;
}

File::~File() {
	close();
}

bool File::open(int access) {
	File::access = access;
	String modestr = getModeString(mode, access);

	if (fileDescriptor == nullptr)
		fileDescriptor = fopen(name.toCharArray(), modestr.toCharArray());
	else
		fileDescriptor = freopen(name.toCharArray(), modestr.toCharArray(), fileDescriptor);

	return fileDescriptor != nullptr;
}

bool File::reopen() {
	if (fileDescriptor == nullptr) {
		return false;
	}

	String modestr = getModeString(mode, access);
	fileDescriptor = freopen(name.toCharArray(), modestr.toCharArray(), fileDescriptor);

	return fileDescriptor != nullptr;
}

bool File::close() {
	if (fileDescriptor == nullptr) {
		return true;
	}

	bool res = fclose(fileDescriptor) == 0;

	fileDescriptor = nullptr;

	return res;
}

bool File::deleteFile() {
	return remove(name.toCharArray()) == 0;
}

int File::deleteFile(const char* name) {
	return remove(name);
}

void File::flush() {
	if (fileDescriptor == nullptr) {
		return;
	}

	fflush(fileDescriptor);
}

FILE* File::getDescriptor() const {
	return fileDescriptor;
}

String File::getModeString(int mode, int access) {
	String str;

	switch (access) {
	case READONLY_ACCESS:
		str = "r";
		break;
	case WRITEABLE_ACCESS:
		str = "w";
		break;
	case APPENDABLE_ACCESS:
		str = "a";
		break;
	default:
		return "";
	}

	if (mode == BINARY_MODE)
		str += "b";

	return str;
}

bool File::mkdir() {
	return doMkdir(name.toCharArray(), permissions);
}

bool File::doMkdir(const char* path, int mode) {
#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

	struct stat st;
	int status = 0;

	if (stat(path, &st) != 0) {
		/* Directory does not exist */
#ifndef PLATFORM_WIN
		if (::mkdir(path, (mode_t)(mode | 0711)) != 0)
#else
		if (_mkdir(path) != 0)
#endif
			status = -1;
	} else if (!S_ISDIR(st.st_mode)) {
		errno = ENOTDIR;
		status = -1;
	}

	return status != -1;
}

char File::directorySeparator() {
	return '/';
}

bool File::mkdirs() {
	return mkpath(getDirName(), permissions | 0110);
}

bool File::mkpath(const String& path, int permissions) {
	char* pp;
	char* sp;
	bool status = true;

	char* copypath = strdup(path.toCharArray());

	pp = copypath;
	while (status && (sp = strchr(pp, '/')) != 0) {
		if (sp != pp) {
			/* Neither root nor double slash in path */
			*sp = '\0';
			status = doMkdir(copypath, permissions);
			*sp = '/';
		}

		pp = sp + 1;
	}

	if (status && pp != copypath)
		status = doMkdir(path.toCharArray(), permissions);

	free(copypath);
	return (status);
}

const String File::getBaseName() const {
	static std::mutex guard;
	std::unique_lock<std::mutex> lock(guard);

	String copypath = name;
	String result;
	auto filename = ::basename(copypath.begin());

	if (filename != nullptr) {
		result = String(filename);
	}

	return result;
}

const String File::getDirName() const {
	static std::mutex guard;
	std::unique_lock<std::mutex> lock(guard);

	String copypath = name;
	String result;
	auto dirname = ::dirname(copypath.begin());

	if (dirname != nullptr) {
		result = String(dirname);
	}

	return result;
}

int File::seek(long offset, int origin) {
	if (fileDescriptor == nullptr) {
		return -1;
	}

	return fseek(fileDescriptor, offset, origin);
}

long File::size() {
	if (fileDescriptor == nullptr) {
		return -1;
	}

	long currentOffset = ftell(fileDescriptor);

	fseek(fileDescriptor, 0L, SEEK_END);

	long size = ftell(fileDescriptor);

	fseek(fileDescriptor, currentOffset, SEEK_SET);

	return size;
}

bool File::setReadOnly() {
	permissions = 0444;

	return open(File::READONLY_ACCESS);
}

bool File::setWriteable() {
	permissions = 0666;

	return open(File::WRITEABLE_ACCESS);
}

bool File::setAppendable() {
	permissions = 0666;

	return open(File::APPENDABLE_ACCESS);
}
