/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ProtectedHeap.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <errno.h>

#ifdef PLATFORM_LINUX
#include <multimmap.h>
#endif

void initializeKernelStatics(int deviceFD);

ProtectedHeap::ProtectedHeap() {
#ifdef MEMORY_PROTECTION
	openDevice(0);

	//heapBase = reinterpret_cast<void*>(2 * MULTIMMAP_HEAP_SIZE);
	setShared();

	offset = MULTIMMAP_KERNEL_HEAP_OFFSET;
#endif
	protectionLevel = 1;

	//linitializeKernelStatics(deviceFD);
}

void ProtectedHeap::openDevice(unsigned deviceNumber) {
    if (deviceFD < 0) {
        char path[256];
        snprintf(path, sizeof(path), "/dev/multimmap%u", deviceNumber);

        //deviceFD = __real_open(path, O_RDWR);
        deviceFD = open(path, O_RDWR);

        if (deviceFD < 0) {
        	char str[100];
            sprintf(str, "[Multimmap] cannot open device file '%s' - %s\n", path, strerror(errno));

            abort();
        }

        printf("[Multimmap] device %s created on fd %u\n", path, deviceFD);
    }

}

void ProtectedHeap::protect() {
	assert(protectionLevel > 0);

	if (protectionLevel > 1)
		protectionLevel--;
	else
		setKernelMemoryProtection(0);
}

void ProtectedHeap::unprotect() {
	if (protectionLevel == 0)
		setKernelMemoryProtection(1);
	else
		protectionLevel++;
}

void initializeKernelStatics(int deviceFD)
{
#ifdef MEMORY_PROTECTION
    multimmap_statics_info statics_info;

    statics_info.start = 0;
    statics_info.size = 512 * 1024 * 1024 / 2;

    if (ioctl(deviceFD, MULTIMMAP_INIT_KERNEL_STATICS, &statics_info) < 0) {
        perror("ioctl");
        fprintf(stderr, "Failed to initialize kernel statics!\n");
        abort();
    }
#endif
}

void ProtectedHeap::setKernelMemoryProtection(unsigned desiredKEL)
{
#ifdef MEMORY_PROTECTION
    struct multimmap_kernel_memory_info memory_info = {
        (unsigned long long) heapBase,
        0,
        &protectionLevel,
        desiredKEL
    };

    if (ioctl(deviceFD, MULTIMMAP_KERNEL_MEMORY_RW, &memory_info)<0) {
        perror("ioctl[MULTIMMAP_KERNEL_MEMORY_RW]");
        abort();
    }
#endif

    //printf("multimmap changed kernel memory protection to %u\n", protectionLevel);
}
