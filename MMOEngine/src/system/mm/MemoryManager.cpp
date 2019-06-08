/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include <sys/mman.h>

#include <errno.h>

#include "MemoryManager.h"

AtomicInteger CloneCounter::cloneCount;

AtomicReference<MemoryManager*> MemoryManager::inst;

void MemoryManager::reclaim(Object* object) {
	object->destroy();
}

void MemoryManager::create(Object* object) {

}

bool MemoryManager::protectForRead(void* mem, size_t size) {
	if (mprotect(mem, size, PROT_READ)) {
		printf("couldn't mprotect %p (%u): %s\n", mem, (unsigned int) size, strerror(errno));

		abort();

		return false;
	}

	return true;
}

bool MemoryManager::protectForWrite(void* mem, size_t size) {
	if (mprotect(mem, size, PROT_WRITE)) {
		perror("Couldn't mprotect");

		return false;
	}

	return true;
}

bool MemoryManager::unprotect(void* mem, size_t size) {
	if (mprotect(mem, size, PROT_READ | PROT_WRITE)) {
		perror("Couldn't mprotect");

		return false;
	}

	return true;
}
