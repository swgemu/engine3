/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "Object.h"

#include "MemoryManager.h"

AtomicReference<MemoryManager> MemoryManager::inst;

void MemoryManager::reclaim(Object* object) {
	object->destroy();
}
