/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include <sys/ioctl.h>

#include "system/lang/StringBuffer.h"

#include "system/io/IOException.h"

#include "FileDescriptor.h"

void FileDescriptor::setBlocking(bool b) {
	#ifndef PLATFORM_WIN
		unsigned long mode = !b;

		int ret = ioctl(fileDescriptor, FIONBIO, &mode );
		if (ret < 0)
			throw IOException("unable to set blocking mode");
	#endif
}
