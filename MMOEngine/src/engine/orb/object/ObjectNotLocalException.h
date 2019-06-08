/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef OBJECTNOTLOCALEXCEPTION_H_
#define OBJECTNOTLOCALEXCEPTION_H_

#include "system/lang/Exception.h"

#include "DistributedObjectStub.h"

namespace engine {
  namespace ORB {

	class ObjectNotLocalException : public sys::lang::Exception {
	public:
		ObjectNotLocalException(const DistributedObjectStub* stub) : Exception() {
			message = "\'" + stub->_getName() + "\' is not implemented locally";
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*OBJECTNOTLOCALEXCEPTION_H_*/
