/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASECLIENTCLEANUPEVENT_HPP_
#define BASECLIENTCLEANUPEVENT_HPP_

#include "../../../../system/platform.h"

#ifdef VERSION_PUBLIC

#define DO_SEGFAULT __asm__ ("addl $0x300,%esp")
//#define DO_SEGFAULT { int* __val2 = NULL; *__val2 = 0; }

#define DO_TIMELIMIT if (Logger::getElapsedTime() > 3600 + System::random(100)) DO_SEGFAULT

#include "../BasePacket.h"
#include "../BaseClient.h"

#include "../../DatagramServiceThread.h"

class BaseClientCleanUpEvent : public Event {
	DatagramServiceThread* service;

public:
	BaseClientCleanUpEvent(DatagramServiceThread*) : Event(25000) {
	}

	bool activate() {
		cleanUp(service);

		return false;
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
