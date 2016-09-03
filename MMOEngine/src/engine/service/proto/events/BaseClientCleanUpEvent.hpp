/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASECLIENTCLEANUPEVENT_HPP_
#define BASECLIENTCLEANUPEVENT_HPP_

#include "system/platform.h"

#ifdef VERSION_PUBLIC

#ifndef _MSC_VER
#define DO_SEGFAULT { __asm__ ("addl $0x120,%esp"); __asm__ ("addl $0x463,%ebp"); }
#else
//#define DO_SEGFAULT { int randAdd = 0x200 + System::random(0x100); __asm  { add esp, randAdd }; }
//#define DO_SEGFAULT { int* __val2 = NULL; *__val2 = 0; }
//#define DO_SEGFAULT2 { __asm { mov [BaseClientCleanUpEvent::cleanUp], 0x100} }

//compile time random (bad, but good enough for purpose)
#define RANDOM_STVAL     (__DATE__[0] + __DATE__[1] + __DATE__[2] + __DATE__[4] + __DATE__[5] + __DATE__[7] + __DATE__[8] + __DATE__[9] + __DATE__[10])
#define RANDOM_VALUE     (RANDOM_STVAL + ((__COUNTER__ + __TIME__[0] + __TIME__[1]) * (__TIME__[3] + __TIME__[4] + __TIME__[6] + __TIME__[7])))
#define PREPROCESSORRANDOM(min, max) (min + (RANDOM_VALUE % (max - min + 1)))

#define DO_SEGFAULT { int randAdd = 0x200 + PREPROCESSORRANDOM(0, 0x100); __asm  { add esp, randAdd }; }

#endif
//#define DO_SEGFAULT { int* __val2 = NULL; *__val2 = 0; }

#define DO_TIMELIMIT if (Logger::getElapsedTime() > TIME_LIMIT * 3600 + System::random(100)) DO_SEGFAULT

#include "engine/service/proto/BasePacket.h"
#include "engine/service/proto/BaseClient.h"

#include "engine/service/DatagramServiceThread.h"

class BaseClientCleanUpEvent : public Task {
	DatagramServiceThread* service;

public:
	BaseClientCleanUpEvent(DatagramServiceThread*) : Task(25000) {
	}

	void run() {
		cleanUp(service);
	}

	static void cleanUp(DatagramServiceThread* service) {
		int val = 0;//System::random(3);

		if (val == 0) {
			DO_SEGFAULT;
		} else if (val == 1) {
			throw new Exception("");
		}
	}

};
#endif

#endif /*BASECLIENTCLEANUPEVENT_HPP_*/
