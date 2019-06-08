/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef RUNNABLE_H_
#define RUNNABLE_H_

namespace sys {
  namespace lang {

	class Runnable {
	public:
		virtual ~Runnable() {
		}
		
		virtual void run() = 0;
		
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*RUNNABLE_H_*/
