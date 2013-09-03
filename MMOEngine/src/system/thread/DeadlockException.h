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

  	  class DeadlockException : public Exception {
  		  Lockable* lockable;
  	  public:
  		  DeadlockException() : Exception(), lockable(NULL) {

  		  }

  		  DeadlockException(const String& message) : Exception(message), lockable(NULL) {

  		  }

  		  DeadlockException(Lockable* lock, const String& message = "") : Exception(message), lockable(lock) {

  		  }

  		  inline Lockable* getLockable() {
  			  return lockable;
  		  }
  	  };
  }
}

using namespace sys::thread;

#endif /* DEADLOCKEXCEPTION_H_ */
