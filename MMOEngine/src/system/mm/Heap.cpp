/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "system/lang/Time.h"

#include "MemoryManager.h"

#include "DLAllocator.h"

#include "Heap.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <limits.h>

#include <multimmap.h>

#include <errno.h>

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

#include "system/thread/Mutex.h"
#include "system/thread/Locker.h"

int Heap::deviceFD = -1;

Heap::Heap() {
	heapBase = reinterpret_cast<void*>(MULTIMMAP_HEAP_SIZE);
	heapSize = -1;
	flags = MAP_PRIVATE;
	offset = 0;

	openDevice(0);
}

Heap::~Heap() {
	delete allocator;

	close(deviceFD);
}

extern int __data_start, _end;

void Heap::create(size_t size) {
	heapSize = size;

	heapBase = mmap(heapBase, heapSize, PROT_READ | PROT_WRITE, flags, deviceFD, offset);
	if (heapBase == reinterpret_cast<void*>(-1)) {
		printf("mmap failed on dev %u with size %u (%s)\n", deviceFD, (unsigned int) heapSize, strerror(errno));

		abort();
	}

	/*if (offset == 0) {
	   multimmap_statics_info statics_info;

	    statics_info.start = reinterpret_cast<unsigned long>(&__data_start);
	    statics_info.start &= ~4095UL;
	    statics_info.size = reinterpret_cast<unsigned long>(&_end) - statics_info.start;

	    if (ioctl(deviceFD, MULTIMMAP_INIT_STATICS, &statics_info) < 0) {
	        perror("ioctl[MULTIMMAP_INIT_STATICS]");
	        abort();
	    }
	}*/

	//printf("heap created at %p on dev %u\n", heapBase, deviceFD);

	if (offset != 0)
		allocator = new DLAllocator(heapBase, heapSize);
}

Time lastPrintTime;
uint64 sum = 0;

/*void Heap::protect() {
	Time start;

	MemoryManager::protectForRead(heapBase, heapSize);

	Time end;

	sum += end.getNanoTime() - start.getNanoTime();

	if (lastPrintTime.miliDifference() > 1000) {
		printf("spent %llu time at mprotect\n", sum / 1000 / 1000);
		lastPrintTime.updateToCurrentTime();

		sum = 0;
	}

	//printf("protected %p, %p\n", heapBase, (void*) ((ptrdiff_t) heapBase + heapSize));
	//printf("mprotect time %llu\n", end.getNanoTime() - start.getNanoTime());
}

void Heap::unprotect() {
	Time start;

	MemoryManager::unprotect(heapBase, heapSize);

	Time end;

	sum += end.getNanoTime() - start.getNanoTime();

	//printf("unprotected %p, %p\n", heapBase, (void*) ((ptrdiff_t) heapBase + heapSize));
	//printf("mprotect time %llu\n", end.getNanoTime() - start.getNanoTime());
}*/

void* Heap::allocate(size_t size) {
	//return malloc(size);
	return allocator->allocate(size);
}

void* Heap::reallocate(void* mem, size_t size) {
	//return realloc(mem, size);
	return allocator->reallocate(mem, size);
}

void Heap::free(void* mem) {
	//free(mem);
	allocator->free(mem);
}

bool Heap::contains(void* mem) {
	ptrdiff_t relativeAddress = (ptrdiff_t) mem - (ptrdiff_t) heapBase;

	return relativeAddress > 0  && relativeAddress <= (ptrdiff_t) heapSize;
}

void Heap::openDevice(unsigned deviceNumber) {
    if (deviceFD < 0) {
        char path[256];
        snprintf(path, sizeof(path), "/dev/multimmap%u", deviceNumber);

        //deviceFD = __real_open(path, O_RDWR);
        deviceFD = open(path, O_RDWR);

        if (deviceFD < 0) {
        	char str[100];
            sprintf(str, "Cannot open device file '%s'\n", path);
            perror(str);

            abort();
        }

        //printf("[multimmap device created on %u\n", deviceFD);
    }

}
