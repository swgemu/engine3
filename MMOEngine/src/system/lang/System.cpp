/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "mersenne/MersenneTwister.h"

#include "System.h"

ThreadLocal<MTRand*> System::mtrand;

PrintStream System::out;

uint32 System::random(unsigned int bucket) {
   	return getMTRand()->randInt(bucket);
}
