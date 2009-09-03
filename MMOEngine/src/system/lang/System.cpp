/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "mersenne/MersenneTwister.h"

#include "System.h"

ThreadLocal<MTRand> System::mtrand;

PrintStream System::out;

uint32 System::random(unsigned int bucket) {
	MTRand* localMT = mtrand.get();

	if (localMT == NULL) {
		localMT = new MTRand();

		mtrand.set(localMT);
	}

   	return localMT->randInt(bucket);
}
