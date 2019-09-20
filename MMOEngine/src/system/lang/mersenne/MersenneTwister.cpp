/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/


#include <iostream>
#include <limits.h>
#include <stdio.h>
#include <math.h>

#include "MersenneTwister.h"

void MTRand::seed() {
	// Seed the generator with an array from /dev/urandom if available
	// Otherwise use a hash of time() and clock() values

	// First try getting an array from /dev/urandom
	FILE* urandom = fopen("/dev/urandom", "rb");
	if (urandom) {
		uint32 bigSeed[N];
		MTRand::uint32 *s = bigSeed;
		int i = N;
		bool success = true;
		while (success && i--)
			success = fread(s++, sizeof(uint32), 1, urandom);
		fclose(urandom);
		if (success) {
			seed(bigSeed, N);
			return;
		}
	}

	// Was not successful, so use time() and clock() instead
	seed(hash(time(nullptr), clock()));
}

std::ostream& operator<<(std::ostream& os, const MTRand& mtrand) {
	const MTRand::uint32 *s = mtrand.state;
	int i = mtrand.N;
	for (; i--; os << *s++ << "\t") {
	}
	return os << mtrand.left;
}

std::istream& operator>>(std::istream& is, MTRand& mtrand) {
	MTRand::uint32 *s = mtrand.state;
	int i = mtrand.N;
	for (; i--; is >> *s++) {
	}
	is >> mtrand.left;
	mtrand.pNext = &mtrand.state[mtrand.N - mtrand.left];
	return is;
}
