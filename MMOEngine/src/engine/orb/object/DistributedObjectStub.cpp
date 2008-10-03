/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../DistributedObjectBroker.h"

#include "DistributedObjectStub.h"

DistributedObjectStub::DistributedObjectStub() : DistributedObject() {
	_impl = NULL;
		
	deployed = false;
	
	#ifdef TRACE_REFERENCING
		finalizedTrace = NULL;
	#endif
}

DistributedObjectStub::~DistributedObjectStub() {
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
	if (_impl == NULL)
		throw Exception("unable to deploy object");
	
	DistributedObjectBroker::instance()->deploy(this);
	
	deployed = true;
}

void DistributedObjectStub::deploy(const char* name) {
	if (_impl == NULL)
		throw Exception("unable to deploy object");

	DistributedObjectBroker::instance()->deploy(name, this);

	deployed = true;
}

void DistributedObjectStub::deploy(const string& name) {
	if (_impl == NULL)
		throw Exception("unable to deploy object");
	
	DistributedObjectBroker::instance()->deploy(name, this);

	deployed = true;
}

void DistributedObjectStub::deploy(const string& name, uint64 nid) {
	stringstream nameid;
	nameid << name << nid;
	
	deploy(nameid.str());
}

bool DistributedObjectStub::undeploy() {
	if (deployed) {
		DistributedObjectBroker::instance()->undeploy(_name);
		deployed = false; 
	} else {
		if (_impl == NULL)
			throw ObjectNotLocalException(this);
		
		DistributedObjectBroker::instance()->info("deleting undeployed implementation");
		
		delete _impl;
		_impl = NULL;
	}
	
	return true;
}

void DistributedObjectStub::finalize() {
	#ifdef TRACE_REFERENCING
		if (finalizedTrace != NULL) {
			cout << "ERROR - object already finalized at\n";
		
			StackTrace::printStackTrace();
		
			cout << "finalized by\n";
		
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
		
	/*stringstream msg;
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

	/*stringstream msg;
	msg << "[" << _getName() << "] released (" << getReferenceCount() - 1 << ")";
	Logger::console.info(msg);*/

	Object::release();
}
	
void DistributedObjectStub::printReferenceTrace() {
	cout << "ERROR - reference count getting under zero\n";

	#ifdef TRACE_REFERENCING
		for (int i = 0; i < traces.size(); ++i) {
			StackTrace* trace = traces.get(i);
		
			Logger::console.info("--------------------------------------------------------------------------", true);
		
			trace->print();
			/*string traceoutput;
			trace->getStackTrace(traceoutput);
		
			Logger::console.info(traceoutput, true);*/
		}
	#endif
}
