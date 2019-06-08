/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/stm/TransactionalMemoryManager.h"

KernelCall::KernelCall() {
	TransactionalMemoryManager::instance()->setKernelMode();
}

KernelCall::~KernelCall() {
	TransactionalMemoryManager::instance()->setUserMode();
}
