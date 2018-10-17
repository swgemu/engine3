/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef NAMEALREADYBOUNDEXCEPTION_H_
#define NAMEALREADYBOUNDEXCEPTION_H_

#include "system/lang/Exception.h"

#include "object/DistributedObjectStub.h"

namespace engine {
  namespace ORB {

	class NameAlreadyBoundException : public sys::lang::Exception {
	public:
		NameAlreadyBoundException(DistributedObjectStub* stub) : Exception() {
			message = "\'" + stub->_getName() + "\' is already deployed";
		}
		
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*NAMEALREADYBOUNDEXCEPTION_H_*/
