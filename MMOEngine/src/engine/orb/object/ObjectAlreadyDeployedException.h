/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECTALREADYDEPLOYEDEXCEPTION_H_
#define OBJECTALREADYDEPLOYEDEXCEPTION_H_

#include "system/lang/Exception.h"

#include "DistributedObjectStub.h"

namespace engine {
  namespace ORB {

	class ObjectAlreadyDeployedException : public sys::lang::Exception {
	public:
		ObjectAlreadyDeployedException(DistributedObjectStub* stub) : Exception() {
			message = "\'" + stub->_getName() + "\' (0x" + String::valueOf(stub->_getObjectID()) + ") is already deployed";
		}

	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*OBJECTALREADYDEPLOYEDEXCEPTION_H_*/
