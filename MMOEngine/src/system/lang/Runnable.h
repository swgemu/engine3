/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
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
