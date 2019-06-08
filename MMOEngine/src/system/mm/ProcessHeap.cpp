/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "ProcessHeap.h"

ProcessHeap::ProcessHeap() {
	heapBase = mmapManager->createHeapBase(0);

	heapID = mmapManager->createHeap();
}

ProcessHeap::~ProcessHeap() {
	mmapManager->destroyHeap(heapID);
}

void ProcessHeap::select() {
	mmapManager->selectHeap(heapID);
}
