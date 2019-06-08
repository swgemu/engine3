/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * AllocationReplacement.cpp
 *
 *  Created on: 23/12/2011
 *      Author: victor
 */

#include "AllocationReplacement.h"

AllocationReplacement* AllocationReplacement::instance = nullptr;

void* AllocationReplacement::onAllocate(size_t size, const void* allocator) {
	return realAllocator->allocate(size);
}

void AllocationReplacement::onFree(void* ptr, const void*) {
	if (ptr == nullptr)
		return;

	realAllocator->free(ptr);
}

void* AllocationReplacement::onReallocate(void* ptr, size_t size, const void* alloc) {
	if (ptr == nullptr)
		return onAllocate(size, alloc);

	return realAllocator->reallocate(ptr, size);
}
