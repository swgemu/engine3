/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
 */

#include "engine/stm/TransactionalMemoryManager.h"

KernelCall::KernelCall() {
	TransactionalMemoryManager::instance()->setKernelMode();
}

KernelCall::~KernelCall() {
	TransactionalMemoryManager::instance()->setUserMode();
}
