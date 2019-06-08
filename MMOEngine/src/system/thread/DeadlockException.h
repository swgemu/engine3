/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * DeadlockException.h
 *
 *  Created on: 03/09/2013
 *      Author: victor
 */

#ifndef DEADLOCKEXCEPTION_H_
#define DEADLOCKEXCEPTION_H_

#include "system/lang/Exception.h"

namespace sys {
  namespace thread {
  	  class Lockable;

  	  class DeadlockException {
  		  Lockable* lockable;
  		  String message;
  		  StackTrace trace;

  	  public:
  		  DeadlockException() : lockable(nullptr) {

  		  }

  		  DeadlockException(const String& msg) : lockable(nullptr), message(msg) {

  		  }

  		  DeadlockException(Lockable* lock, const String& msg = "") : lockable(lock), message(msg) {

  		  }

  		  inline StackTrace* getStackTrace() {
  			  return &trace;
  		  }

  		  inline void printStackTrace() {
  			  trace.print();
  		  }

  		  inline Lockable* getLockable() {
  			  return lockable;
  		  }

  		  inline String getMessage() {
  			  return message;
  		  }
  	  };
  }
}

using namespace sys::thread;

#endif /* DEADLOCKEXCEPTION_H_ */
