/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DISTRIBUTEDOBJECTCLASSHELPER_H_
#define DISTRIBUTEDOBJECTCLASSHELPER_H_

#include "DistributedObject.h"
#include "DistributedObjectServant.h"
#include "DistributedObjectAdapter.h"

namespace engine {
  namespace ORB {

	class DistributedObjectStub;
	
	class DistributedObjectClassHelper : public Object {
	protected:
		String className;
		
	public:
		DistributedObjectClassHelper() {
		}
		
		virtual ~DistributedObjectClassHelper() {
		}	
	
		virtual void finalizeHelper() = 0;
		
		virtual DistributedObject* instantiateObject() = 0;

		virtual DistributedObjectServant* instantiateServant() = 0;

		virtual DistributedObjectAdapter* createAdapter(DistributedObjectStub* obj) = 0;
		
		inline const String& getClassName() {
			return className;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTCLASSHELPER_H_*/
