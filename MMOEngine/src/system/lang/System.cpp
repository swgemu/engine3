/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "System.h"

ThreadLocal<MTRand*> System::mtrand;

TextStream System::out(stdout);
TextStream System::err(stderr);

uint32 System::random(uint32 bucket) {
   	return getMTRand()->randInt(bucket);
}

uint32 System::random() {
   	return getMTRand()->randInt();
}

double System::frandom(const double& n) {
   	return getMTRand()->rand(n);
}

double System::frandom() {
   	return getMTRand()->rand();
}

MTRand* System::getMTRand() {
	MTRand* localMT = mtrand.get();

	if (localMT == nullptr) {
		localMT = new MTRand();

		mtrand.set(localMT);
	}

	return localMT;
}

