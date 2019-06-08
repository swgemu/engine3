/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "ProtectedHeap.h"

ProtectedHeap::ProtectedHeap() {
	offset = (off_t) mmapManager->getKernelHeapBase();

	protectionLevel = 1;

	setShared();
}

void ProtectedHeap::protect() {
	assert(protectionLevel > 0);

	if (protectionLevel > 1)
		protectionLevel--;
	else
		mmapManager->setKernelMemoryProtection((unsigned long long) heapBase, protectionLevel, 0);
}

void ProtectedHeap::unprotect() {
	if (protectionLevel == 0)
		mmapManager->setKernelMemoryProtection((unsigned long long) heapBase, protectionLevel, 1);
	else
		protectionLevel++;
}
