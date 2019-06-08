/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "DLAllocator.h"

#define USE_DL_PREFIX 1
#define USE_LOCKS 1
#define MSPACES 1
//#define HAVE_MMAP 0
#undef _GNU_SOURCE
#include "dlmalloc.h"

DLAllocator::DLAllocator(void* base, size_t size) {
	dlMspace = nullptr;

	dlBase = base;
	dlSize = size;

	initialize();
}

DLAllocator::~DLAllocator() {
	destroy();
}

void DLAllocator::initialize() {
	printf("creating dlallocator at %p with size %zu\n", dlBase, dlSize);

	dlMspace = create_mspace_with_base(dlBase, dlSize, 0);
}

void DLAllocator::destroy() {
	destroy_mspace((mspace) dlMspace);
}

void* DLAllocator::allocate(size_t size) {
	return mspace_malloc((mspace) dlMspace, size);
}

void* DLAllocator::reallocate(void* mem, size_t newsize) {
	return mspace_realloc((mspace) dlMspace, mem, newsize);
}

void DLAllocator::free(void* mem) {
	mspace_free((mspace) dlMspace, mem);
}

size_t DLAllocator::sizeOf(void* mem) {
	return chunksize(mem2chunk(mem));
}
