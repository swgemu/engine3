/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DISTRIBUTEDOBJECTSERVANT_H_
#define DISTRIBUTEDOBJECTSERVANT_H_

#include "system/lang.h"

namespace engine {
  namespace ORB {

  	class DistributedObjectStub;

  	class DistributedObjectClassHelper;

	class DistributedObjectServant : public Object {
	protected:
		DistributedObjectClassHelper* _classHelper = nullptr;

		WeakReference<DistributedObjectStub*> _stub;

	public:
		DistributedObjectServant();

		virtual ~DistributedObjectServant();

		void setDeployingName(const String& name);

		virtual void _setStub(DistributedObjectStub* stub) {
			_stub = stub;
		}

		virtual void _serializationHelperMethod() = 0;

		virtual DistributedObjectStub* _getStub() = 0;

		String toString() const;

		// setters
		inline void _setClassHelper(DistributedObjectClassHelper* helper) {
			_classHelper = helper;
		}

		// getters
		inline DistributedObjectClassHelper* _getClassHelper() {
			return _classHelper;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTSERVANT_H_*/
