/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "system/lang/Time.h"

#include "MemoryManager.h"

#include "DLAllocator.h"

#include "Heap.h"

#include <sys/mman.h>
#include <limits.h>

#ifdef PLATFORM_LINUX
#include <multimmap.h>
#endif

#include <errno.h>

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

#include "system/thread/Mutex.h"
#include "system/thread/Locker.h"

AtomicInteger Heap::heapCount;

Heap::Heap() {
#ifdef PLATFORM_LINUX
	int count = heapCount.increment();

	heapBase = reinterpret_cast<void*>(MULTIMMAP_HEAP_SIZE * count);

	heapSize = -1;
	offset = 0;

	deviceFD = -1;

	setPrivate();
#endif
}

Heap::Heap(int fd) {
#ifdef PLATFORM_LINUX
	int count = heapCount.increment();

	heapBase = reinterpret_cast<void*>(MULTIMMAP_HEAP_SIZE * count);
	heapSize = -1;
	offset = 0;

	deviceFD = fd;

	setPrivate();
#endif
}

Heap::~Heap() {
	delete allocator;

	close(deviceFD);
}

extern int __data_start, _end;

void Heap::create(size_t size) {
#ifdef PLATFORM_LINUX
	heapSize = size;

	heapBase = mmap(heapBase, heapSize, PROT_READ | PROT_WRITE, flags, deviceFD, offset);
	if (heapBase == reinterpret_cast<void*>(-1)) {
		printf("mmap failed on dev %i with size %lu (%s)\n", deviceFD, heapSize, strerror(errno));

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

	printf("[Heap] created at %p-%p on fd %i\n", heapBase, (void*)((uintptr_t) heapBase + heapSize), deviceFD);

	//if (offset != 0)
		allocator = new DLAllocator(heapBase, heapSize);
#endif
}

Time lastPrintTime;
uint64 sum = 0;

void* Heap::allocate(size_t size) {
	return allocator->allocate(size);
}

void* Heap::reallocate(void* mem, size_t size) {
	return allocator->reallocate(mem, size);
}

void Heap::free(void* mem) {
	allocator->free(mem);
}

size_t Heap::sizeOf(void* mem) {
	return allocator->sizeOf(mem);
}

bool Heap::contains(void* mem) {
	ptrdiff_t relativeAddress = (ptrdiff_t) mem - (ptrdiff_t) heapBase;

	return relativeAddress > 0  && relativeAddress <= (ptrdiff_t) heapSize;
}

void Heap::setShared() {
	flags = MAP_SHARED;
}

void Heap::setPrivate() {
	flags = MAP_PRIVATE;
}

void Heap::setAnonymous() {
	flags |= MAP_ANONYMOUS;
}
