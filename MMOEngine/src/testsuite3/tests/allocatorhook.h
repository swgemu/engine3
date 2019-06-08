/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * allocatorhook.h
 *
 *  Created on: 23/12/2011
 *      Author: victor
 */

#ifndef ALLOCATORHOOK_H_
#define ALLOCATORHOOK_H_

#include "system/mm/AllocationReplacement.h"
#include "system/mm/MemoryMappedFile.h"

void testMemoryMappedFile() {
	MemoryMappedFile* mmapped = new MemoryMappedFile("memory_mapped_file.mm", 512 * 4096);
	mmapped->create();
	AllocationReplacement* replace = AllocationReplacement::getInstance();
	replace->setRealAllocator(mmapped->getAllocator());

	printf("installing alloc hook\n");

	replace->install();

	printf("hook installed\n");

	printf("creating object\n");

	Object* obj = new Object();

	printf("%d created object with allocation hooks on and mmapped file\n", obj->getReferenceCount());

	replace->uninstall();

	printf("done\n");
}

#endif /* ALLOCATORHOOK_H_ */
