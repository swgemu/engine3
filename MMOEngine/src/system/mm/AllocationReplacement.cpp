/*
 * AllocationReplacement.cpp
 *
 *  Created on: 23/12/2011
 *      Author: victor
 */

#include "AllocationReplacement.h"

AllocationReplacement* AllocationReplacement::instance = NULL;

void* AllocationReplacement::onAllocate(size_t size, const void* allocator) {
	return realAllocator->allocate(size);
}

void AllocationReplacement::onFree(void* ptr, const void*) {
	if (ptr == NULL)
		return;

	realAllocator->free(ptr);
}

void* AllocationReplacement::onReallocate(void* ptr, size_t size, const void* alloc) {
	if (ptr == NULL)
		return onAllocate(size, alloc);

	return realAllocator->reallocate(ptr, size);
}
