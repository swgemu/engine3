/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DISTRIBUTEDOBJECTSTUB_H_
#define DISTRIBUTEDOBJECTSTUB_H_

#include "engine/util/Singleton.h"

#include "DistributedObjectServant.h"

namespace engine {
  namespace ORB {

	class DistributedObjectStub : public DistributedObject {
	protected:
		DistributedObjectServant* _impl;
		
		bool deployed;
		
		DistributedObjectClassHelper* _classHelper;
		
	#ifdef TRACE_REFERENCING
		Vector<StackTrace*> traces;
		
		StackTrace* finalizedTrace;
	#endif
				
	public:
		DistributedObjectStub();

		virtual ~DistributedObjectStub();

		//virtual DistributedObjectStub* clone();

		// deployment methods
		void deploy();
		void deploy(const char* name);
		void deploy(const string& name);
		void deploy(const string& name, sys::uint64 nid);

		bool undeploy();

		// reference counting methods
		virtual void finalize();

		virtual void acquire();
	
		virtual void release();
			
		void printReferenceTrace();
		
		// setters
		inline void setDeployingName(const string& name) {
			_setName(name);
		}
		
		inline void _setClassHelper(DistributedObjectClassHelper* helper) {
			_classHelper = helper;
		}
		
		// getters
		inline bool isDeplyoed() {
			return deployed;
		}
		
		inline DistributedObjectServant* _getImplementation() {
			return _impl;
		}
		
		inline DistributedObjectClassHelper* _getClassHelper() {
			return _classHelper;
		}	
	};

	class DummyConstructorParameter : public Singleton<DummyConstructorParameter> {
		friend class SingletonWrapper<DummyConstructorParameter>;
	};
	
  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTSTUB_H_*/
