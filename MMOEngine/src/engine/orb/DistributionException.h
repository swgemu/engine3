/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DISTRIBUTIONEXCEPTION_H_
#define DISTRIBUTIONEXCEPTION_H_

#include "system/lang/Exception.h"

#include "object/DistributedObjectStub.h"

namespace engine {
  namespace ORB {

	class DistributionException : public sys::lang::Exception {
	public:
		DistributionException() : Exception() {
			message = "remote node is unreachable";
			assert(0);
		}

		DistributionException(uint64 objectID) : Exception() {
			message = "object 0x" + String::valueOf(objectID) + " is unreachable";
			assert(0);
		}

		DistributionException(const String& name) : Exception() {
			message = "object \'" + name + "\' is unreachable";
			assert(0);
		}

		DistributionException(DistributedObjectStub* stub) : Exception() {
			message = "object \'" + stub->_getName() + "\' is unreachable";
			assert(0);
		}
		
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTIONEXCEPTION_H_*/
