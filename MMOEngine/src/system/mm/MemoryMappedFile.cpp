/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "MemoryMappedFile.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <errno.h>

MemoryMappedFile::MemoryMappedFile(const char* name) {
	fileName = name;

	openDevice();
}

void MemoryMappedFile::openDevice() {
	deviceFD = open(fileName, O_RDWR);

	if (deviceFD < 0) {
		char str[100];
		sprintf(str, "[MemoryMappedFiles] cannot open device file '%s' - %s\n", fileName, strerror(errno));
	}

	printf("[MemoryMappedFiles] file %s created on fd %u\n", fileName, deviceFD);
}
