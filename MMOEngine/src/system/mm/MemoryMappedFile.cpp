/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "MemoryMappedFile.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <errno.h>

MemoryMappedFile::MemoryMappedFile(const char* name, uint32 size) {
	fileName = name;

	heapBase = NULL;

	fileSize = size;

	setShared();

	openDevice();
}

void MemoryMappedFile::openDevice() {
	deviceFD = open(fileName, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);

	if (deviceFD < 0) {
		char str[100];
		sprintf(str, "[MemoryMappedFiles] cannot open device file '%s' - %s\n", fileName, strerror(errno));
	} else {
		lseek(deviceFD, fileSize - 1, SEEK_SET);

		char shit = '\0';
		int res = write(deviceFD, &shit, 1);

		if (res < 0)
			printf("could not enlarge file %s to size %d\n", fileName, fileSize);
		//printf("%d result of write\n", res);

	}

	printf("[MemoryMappedFiles] file %s created on fd %u\n", fileName, deviceFD);
}
