/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "System.h"

ThreadLocal<MTRand*> System::mtrand;

PrintStream System::out;

uint32 System::random(unsigned int bucket) {
   	return getMTRand()->randInt(bucket);
}

MTRand* System::getMTRand() {
	MTRand* localMT = mtrand.get();

	if (localMT == nullptr) {
		localMT = new MTRand();

		mtrand.set(localMT);
	}

	return localMT;
}

