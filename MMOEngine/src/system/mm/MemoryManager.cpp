/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "MemoryManager.h"

#include "system/mm/Heap.h"
#include "system/mm/ProtectedHeap.h"

#ifndef PLATFORM_WIN
#include <sys/mman.h>
#endif

#include <errno.h>

AtomicInteger CloneCounter::cloneCount;

AtomicReference<MemoryManager*> MemoryManager::inst;

void MemoryManager::reclaim(Object* object) {
	object->destroy();
}

void MemoryManager::create(Object* object) {

}

bool MemoryManager::protectForRead(void* mem, size_t size) {
#ifndef PLATFORM_WIN
	if (mprotect(mem, size, PROT_READ)) {
		printf("couldn't mprotect %p (%u): %s\n", mem, (unsigned int) size, strerror(errno));

		abort();

		return false;
	}
#endif

	return true;
}

bool MemoryManager::protectForWrite(void* mem, size_t size) {
#ifndef PLATFORM_WIN
	if (mprotect(mem, size, PROT_WRITE)) {
		perror("Couldn't mprotect");

		return false;
	}

#endif

	return true;
}

bool MemoryManager::unprotect(void* mem, size_t size) {
#ifndef PLATFORM_WIN
	if (mprotect(mem, size, PROT_READ | PROT_WRITE)) {
		perror("Couldn't mprotect");

		return false;
	}
#endif

	return true;
}

