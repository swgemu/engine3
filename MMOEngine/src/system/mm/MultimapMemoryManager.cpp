/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "MultimapMemoryManager.h"

#ifdef PLATFORM_LINUX
#include <multimmap.h>
#endif

AtomicReference<MultimapMemoryManager*> MultimapMemoryManager::inst;

AtomicInteger MultimapMemoryManager::heapCount;

extern int __data_start, _end;

MultimapMemoryManager::~MultimapMemoryManager() {
	close(deviceFD);
}

void MultimapMemoryManager::initialize() {
	deviceFD = -1;

#ifdef MEMORY_PROTECTION
	openDevice(0);

	multimmap_statics_info statics_info;

	statics_info.start = reinterpret_cast<unsigned long>(&__data_start);
	statics_info.start &= ~4095UL;
	statics_info.size = reinterpret_cast<unsigned long>(&_end) - statics_info.start;

	if (ioctl(deviceFD, MULTIMMAP_INIT_STATICS, &statics_info) < 0) {
		perror("ioctl[MULTIMMAP_INIT_STATICS]");
		abort();
	}
#endif
}

void MultimapMemoryManager::openDevice(unsigned deviceNumber) {
#ifdef MEMORY_PROTECTION
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
#endif
}

void* MultimapMemoryManager::createHeapBase(size_t size) {
	int count = heapCount.increment();

#ifdef MEMORY_PROTECTION
	void* base = reinterpret_cast<void*>(MULTIMMAP_HEAP_SIZE * count);

	base = mmap(base, size, PROT_READ | PROT_WRITE, flags, deviceFD, offset);
	if (base == reinterpret_cast<void*>(-1)) {
		printf("mmap failed on dev %i with size %zu (%s)\n", deviceFD, size, strerror(errno));

		abort();
	}

	return base;
#else
	return reinterpret_cast<void*>(512*512*512*4096UL * count);
#endif
}

void* MultimapMemoryManager::getKernelHeapBase() {
#ifdef MEMORY_PROTECTION
	return MULTIMMAP_KERNEL_HEAP_OFFSET;
#else
	return 0;
#endif
}

void MultimapMemoryManager::initializeKernel()
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

void MultimapMemoryManager::setKernelMemoryProtection(unsigned long long heapBase, volatile unsigned& protectionLevel, unsigned desiredKEL)
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

    //printf("multimmap changed kernel memory protection to %u\n", protectionLevel);
#endif
}

int MultimapMemoryManager::createHeap()
{
#ifdef MEMORY_PROTECTION
	int heapID = ioctl(deviceFD, MULTIMMAP_CREATE);
	if (heapID < 0) {
		perror("multimmap create");
		abort();
	}

	return heapID;
#else
	return -1;
#endif
}

void MultimapMemoryManager::selectHeap(int heapID) {
#ifdef MEMORY_PROTECTION
	if (ioctl(deviceFD, MULTIMMAP_SELECT, heapID) < 0) {
		perror("multimmap select");
		abort();
	}
#endif
}

void MultimapMemoryManager::destroyHeap(int heapID)
{
#ifdef MEMORY_PROTECTION
	ioctl(deviceFD, MULTIMMAP_DESTROY, heapID);
#endif
}
