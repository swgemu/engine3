/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "system/lang/Time.h"

#include "MemoryManager.h"

#include "DLAllocator.h"

#include <sys/mman.h>

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

Heap::Heap() {
	mmapManager = MultimapMemoryManager::getInstance();

	heapBase = reinterpret_cast<void*>(-1);
	heapSize = -1;

	offset = 0;

	setPrivate();
}

Heap::~Heap() {
	delete allocator;
}

void Heap::create(size_t size) {
	heapBase = mmapManager->createHeapBase(size);

	heapSize = size;

	printf("[Heap] created at %p-%p on fd %i\n", heapBase, (void*)((uintptr_t) heapBase + heapSize),
			mmapManager->getDeviceFD());

	//if (offset != 0)
		allocator = new DLAllocator(heapBase, heapSize);
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
#ifdef PLATFORM_MAC
	flags |= MAP_ANON;
#else
	flags |= MAP_ANONYMOUS;
#endif
}
