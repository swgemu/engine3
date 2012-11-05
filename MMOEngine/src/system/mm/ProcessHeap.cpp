/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
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
