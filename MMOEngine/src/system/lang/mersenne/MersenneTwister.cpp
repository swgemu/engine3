#include "MersenneTwister.h"

inline MTRand::MTRand(const MTRand::uint32& oneSeed) {
	mutex = new Mutex();

	seed(oneSeed);
}

inline MTRand::MTRand(MTRand::uint32 * const bigSeed, const MTRand::uint32 seedLength) {
	mutex = new Mutex();

	seed(bigSeed, seedLength);
}

inline MTRand::MTRand() {
	mutex = new Mutex();

	seed();
}

inline MTRand::uint32 MTRand::randInt(const MTRand::uint32& n) {
	mutex->lock();

	// Find which bits are used in n
	// Optimized by Magnus Jonsson (magnus@smartelectronix.com)
	uint32 used = n;
	used |= used >> 1;
	used |= used >> 2;
	used |= used >> 4;
	used |= used >> 8;
	used |= used >> 16;

	// Draw numbers until one is found in [0,n]
	uint32 i;
	do
		i = randInt() & used; // toss unused bits to shorten search
	while (i > n);

	mutex->unlock();

	return i;
}

