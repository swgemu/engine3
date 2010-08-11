/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../DistributedObjectBroker.h"

#include "DistributedObjectStub.h"

DistributedObjectStub::DistributedObjectStub() : DistributedObject() {
	_setImplementation(NULL);
		
	deployed = false;

	_classHelper = NULL;

	destroyed = false;

	#ifdef TRACE_REFERENCING
		finalizedTrace = NULL;
	#endif
}

DistributedObjectStub::~DistributedObjectStub() {
	if (!destroyed)
		undeploy();
	
	#ifdef TRACE_REFERENCING
		for (int i = 0; i < traces.size(); ++i) {
			StackTrace* trace = traces.get(i);
			delete trace;
		}
	
		delete finalizedTrace;
		finalizedTrace = NULL;
	#endif
}

/*DistributedObjectStub* DistributedObjectStub::clone() {
	return new DistributedObjectStub(*this);
}*/

void DistributedObjectStub::deploy() {
	if (_getImplementation() == NULL)
		throw Exception("unable to deploy object");
	
	DistributedObjectBroker::instance()->deploy(this);
	
	deployed = true;

	destroyed = false;
}

void DistributedObjectStub::deploy(const char* name) {
	if (_getImplementation() == NULL)
		throw Exception("unable to deploy object");

	DistributedObjectBroker::instance()->deploy(name, this);

	deployed = true;

	destroyed = false;
}

void DistributedObjectStub::deploy(const String& name) {
	if (_getImplementation() == NULL)
		throw Exception("unable to deploy object");
	
	DistributedObjectBroker::instance()->deploy(name, this);

	deployed = true;

	destroyed = false;
}

void DistributedObjectStub::deploy(const String& name, uint64 nid) {
	StringBuffer nameid;
	nameid << name << nid;
	
	deploy(nameid.toString());

	destroyed = false;
}

bool DistributedObjectStub::undeploy() {
	if (deployed) {
		DistributedObjectBroker* broker = DistributedObjectBroker::instance();

		if (broker != NULL)
			broker->undeploy(_name);

		deployed = false; 
	} else {
		if (_getImplementation() == NULL)
			throw ObjectNotLocalException(this);
		
		DistributedObjectBroker::instance()->info("deleting undeployed implementation");
		
		delete _getImplementation();
		_setImplementation(NULL);
	}
	
	destroyed = true;

	return true;
}

void DistributedObjectStub::finalize() {
	#ifdef TRACE_REFERENCING
		if (finalizedTrace != NULL) {
			System::out << "ERROR - object already finalized at\n";
		
			StackTrace::printStackTrace();
		
			System::out << "finalized by\n";
		
			finalizedTrace->print();
	
			raise(SIGSEGV);		
		}

		finalizedTrace = new StackTrace();
	#endif

	Object::finalize();
}

void DistributedObjectStub::acquire() {
	#ifdef TRACE_REFERENCING
		traces.add(new StackTrace());
	#endif
		
	Object::acquire();
		
	/*StringBuffer msg;
	msg << "[" << _getName() << "] acquired (" << getReferenceCount() << ")";
	Logger::console.info(msg);*/
}
	
void DistributedObjectStub::release() {
	#ifdef TRACE_REFERENCING
		traces.add(new StackTrace());

		if (getReferenceCount() < 1) {
			printReferenceTrace();

			raise(SIGSEGV);
		}
	#endif

	/*StringBuffer msg;
	msg << "[" << _getName() << "] released (" << getReferenceCount() - 1 << ")";
	Logger::console.info(msg);*/

	Object::release();
}
	
void DistributedObjectStub::printReferenceTrace() {
	System::out << "ERROR - reference count getting under zero\n";

	#ifdef TRACE_REFERENCING
		for (int i = 0; i < traces.size(); ++i) {
			StackTrace* trace = traces.get(i);
		
			Logger::console.info("--------------------------------------------------------------------------", true);
		
			trace->print();
			/*String traceoutput;
			trace->getStackTrace(traceoutput);
		
			Logger::console.info(traceoutput, true);*/
		}
	#endif
}
