/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DISTRIBUTEDOBJECTSTUB_H_
#define DISTRIBUTEDOBJECTSTUB_H_

#include "engine/util/Singleton.h"

#include "DistributedObject.h"
#include "DistributedObjectServant.h"

namespace engine {
  namespace ORB {

	class DistributedObjectStub : public DistributedObject {
	protected:
		Reference<DistributedObjectServant*> _impl;

		bool deployed;

		DistributedObjectClassHelper* _classHelper;

		bool destroyed;

	#ifdef TRACE_REFERENCING
		Vector<StackTrace*> traces;

		StackTrace* finalizedTrace;
	#endif

	public:
		DistributedObjectStub();

		~DistributedObjectStub();

		//virtual DistributedObjectStub* clone();

		// deployment methods
		void deploy();
		void deploy(const char* name);
		void deploy(const String& name);
		void deploy(const String& name, sys::uint64 nid);

		void _requestServant();

		bool undeploy();

		// reference counting methods
		virtual void finalize();

		//virtual void acquire();

		//virtual void release();

		void printReferenceTrace() const;

		// setters
		inline void setDeployingName(const String& name) {
			_setName(name);
		}

		inline void _setClassHelper(DistributedObjectClassHelper* helper) {
			_classHelper = helper;
		}

		inline void setDeployed(bool val) {
			deployed = val;
		}

		virtual void _setImplementation(DistributedObjectServant* impl) {
			_impl = impl;
		}

		// getters
		inline bool isDeployed() const {
			return deployed;
		}

		virtual DistributedObjectServant* _getImplementation() {
			return _impl;
		}

		virtual DistributedObjectServant* _getImplementationForRead() const {
			return _impl;
		}

		inline DistributedObjectClassHelper* _getClassHelper() {
			return _classHelper;
		}

		virtual DistributedObjectServant* getServant() {
			return _impl;
		}
	};

	class DummyConstructorParameter : public Singleton<DummyConstructorParameter>, public Object {
		friend class SingletonWrapper<DummyConstructorParameter>;
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTSTUB_H_*/
