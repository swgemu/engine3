/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "MemoryManager.h"

#include "AllocationTracker.h"

AllocationTracker* AllocationTracker::instance = nullptr;

AllocationTracker::AllocationTracker() {
	realAllocator = nullptr;

	numOfAllocatorsM = 0;

	memset(allocatorsM, 0, sizeof(allocatorsM));
}

AllocationTracker::~AllocationTracker() {
}

void* AllocationTracker::onAllocate(size_t size, const void* allocator) {
	size += sizeof(uint64);

	//uint64* ptr = (uint64*) unHookedAllocate(size);
	uint64* ptr = (uint64*) realAllocator->allocate(size);

	addOverWriteProtector(ptr, size);

	//updateAllocatorList((unsigned long) allocator, size);

	//*(ptr + size - sizeof(uint64)) = (uint64) allocator;
	//*(ptr + size - 2 * sizeof(uint64)) = (uint64) size;

	return ptr;
}

void AllocationTracker::onFree(void* ptr, const void*) {
	if (ptr == nullptr)
		return;

	uint64* lptr = (uint64*) ptr;

	size_t size = realAllocator->sizeOf(ptr);
	//void* allocator = (void*) *(lptr + size - sizeof(uint64));

	//updateAllocatorList((unsigned long) allocator, -size);

	//unHookedFree((void*) lptr);
	realAllocator->free((void*) lptr);
}

void* AllocationTracker::onReallocate(void* ptr, size_t size, const void* alloc) {
	if (ptr == nullptr)
		return onAllocate(size, alloc);

	uint64* lptr = (uint64*) ptr;

	size += sizeof(uint64);

	size_t old_chunk_size = realAllocator->sizeOf(ptr);
	//void* allocator = (void*) *(lptr + old_chunk_size - sizeof(uint64));

	//void* ptr2 = unHookedReallocate((void*)lptr, size);
	void* ptr2 = realAllocator->reallocate((void*)lptr, size);

	uint64* nlptr = (uint64*) ptr2;
	//*(nlptr + size - sizeof(uint64)) = (uint64) allocator;
	//*(nlptr++) = size;

	//updateAllocatorList((unsigned long)allocator, -(old_chunk_size - size));

	return nlptr;
}

size_t AllocationTracker::sizeOfPointer(void* ptr) {
	return realAllocator->sizeOf(ptr);
}

void AllocationTracker::updateAllocatorList(unsigned long allocator, long size) {
	if (nextAllocatorPrint.isPast()) {
		print();
		nextAllocatorPrint.addMiliTime(10000);
	}

	for(unsigned int i = 0; i < numOfAllocatorsM; i++)
	{
		if(allocatorsM[i].caller == allocator)
		{
			 allocatorsM[i].total += size;
			 return;
		}
	}
	if(numOfAllocatorsM < MM_ALLOCATOR_LIST_SIZE)
	{
		allocatorsM[numOfAllocatorsM].caller = allocator;
		allocatorsM[numOfAllocatorsM].total  = size;
		numOfAllocatorsM++;
	}
	else
	{
		printf("Allocator debug, too many allocators!!!!\n");
		assert(0);
	}
}

void AllocationTracker::print() {
	printf("--- ALLOCATORS ---\n");
	unsigned long grand_total = 0;
	for(unsigned int i = 0; i < numOfAllocatorsM; i++)
	{
		if(allocatorsM[i].total > 0)
		{
			printf("#%4u 0x%08lx %ld\n", i, allocatorsM[i].caller, allocatorsM[i].total);
		}
		else if(allocatorsM[i].total < 0)
		{
			printf("#%4u 0x%08lx %ld ???\n", i, allocatorsM[i].caller, allocatorsM[i].total);
		}
		grand_total += allocatorsM[i].total;
	}
	printf("Total: %lu (%u)\n", grand_total, numOfAllocatorsM);
	printf("--- ALLOCATORS ---\n");
	fflush(stdout);
}

void AllocationTracker::addOverWriteProtector(void* ptr, size_t size) {
	uintptr_t pageoffsbeg = (uintptr_t) ptr & (4096 - 1);
	uintptr_t pageoffsend = ((uintptr_t) ptr + size) & (4096 - 1);

	if (pageoffsbeg == pageoffsend || protectedPagesCount >= 5000) {
		return;
	}

	int* page = (int*) realAllocator->allocate(4096 + 4096 - 1);
	page = (int *)(((uintptr_t)(page) + 4096 - 1) & ~(4096 - 1));

	assert(((uintptr_t) page) % 4096 == 0);

	MemoryManager::protectForRead(page, 4096);

	protectedPages[protectedPagesCount++] = (uint64) page;

	//ptr[20] = 34;
}
