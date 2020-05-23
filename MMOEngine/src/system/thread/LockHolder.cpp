/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "LockHolder.h"

namespace sys {
namespace thread {
	AtomicInteger LockHolder::lockSequence;

	const String LockHolder::format(char type, int16 threadNumber, const char* file, const char* function, int line, uint32 sequence) {
		const char *cp = file;

		while (*cp) {
			cp++;
		}

		cp--;

		for (int depth = 3;cp > file && depth > 0; cp--) {
			if (*cp == '/' && --depth <= 0) {
				cp++;
				break;
			}
		}

		StringBuffer buf;

		if (sequence != 0) {
			buf << "(#" << sequence << ") ";
		}

		buf << "[Thread " << threadNumber << "] " << type
			<< "Lock " << cp
			<< ":" << line
			<< " " << (function == nullptr ? "unknown" : function)
			<< "()";

		return buf.toString();
	}
}
}
