/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/service/proto/BasePacketHandler.h"

#include "DistributedObjectBroker.h"

#include "DistributedObjectBrokerClient.h"

#include "NamingDirectoryServiceImpl.h"

DistributedObjectBroker::DistributedObjectBroker()
		: StreamServiceThread("DistributedObjectBroker") {
	phandler = NULL;

	namingDirectoryInterface = NULL;

	setLogging(false);
}

DistributedObjectBroker::~DistributedObjectBroker() {
	shutdown();

	if (namingDirectoryInterface != NULL) {
		delete namingDirectoryInterface;
		namingDirectoryInterface = NULL;
	}

	if (phandler != NULL) {
		delete phandler;
		phandler = NULL;
	}
}

DistributedObjectBroker* DistributedObjectBroker::initialize(const String& addr, int port) {
	DistributedObjectBroker* inst = instance();

	inst->address = addr;
	inst->start(port);

	return inst;
}

void DistributedObjectBroker::init() {
	if (address.isEmpty()) {
		namingDirectoryInterface = new NamingDirectoryServiceImpl();

		info("root naming directory initialized", true);
	} else {
		namingDirectoryInterface = new NamingDirectoryServiceImpl(address);
	}

	phandler = new DOBPacketHandler("ORBPacketHandler", this);
	phandler->setLogging(false);
}

void DistributedObjectBroker::run() {
	scheduler->start();

	acceptConnections();
}

void DistributedObjectBroker::shutdown() {
	stop();

	scheduler->stop();
}

DistributedObjectBrokerClient* DistributedObjectBroker::createConnection(Socket* sock, SocketAddress& addr) {
	DistributedObjectBrokerClient* client = new DistributedObjectBrokerClient(this, sock);
	client->start();

	return client;
}

void DistributedObjectBroker::registerClass(const String& name, DistributedObjectClassHelper* helper) {
	lock();

	try {
		classMap.put(name, helper);
	} catch (...) {
		error("unreported Exception on registerClass()");
	}

	unlock();
}

void DistributedObjectBroker::deploy(DistributedObjectStub* obj) {
	lock();

	DistributedObjectServant* servant = obj->_getImplementation();
	if (servant == NULL) {
		unlock();

		throw ObjectNotLocalException(obj);
	}

	try {
		DistributedObjectClassHelper* helper = servant->_getClassHelper();
		DistributedObjectAdapter* adapter = helper->createAdapter(obj);

		namingDirectoryInterface->deploy(obj);

		objectDirectory.add(obj->_getObjectID(), adapter);

		info("object \'" + obj->_getName() + "\' deployed");
	} catch (Exception& e) {
		error(e.getMessage());
	} catch (...) {
		error("unreported Exception on deploy()");
	}

	unlock();
}

void DistributedObjectBroker::deploy(const String& name, DistributedObjectStub* obj) {
	lock();

	DistributedObjectServant* servant = obj->_getImplementation();
	if (servant == NULL) {
		unlock();

		throw ObjectNotLocalException(obj);
	}

	try {
		DistributedObjectClassHelper* helper = servant->_getClassHelper();
		DistributedObjectAdapter* adapter = helper->createAdapter(obj);

		namingDirectoryInterface->deploy(name, obj);

		objectDirectory.add(obj->_getObjectID(), adapter);

		info("object \'" + obj->_getName() + "\' deployed");
	} catch (Exception& e) {
		error(e.getMessage());
	} catch (...) {
		error("unreported Exception on deploy()");
	}

	unlock();
}

DistributedObject* DistributedObjectBroker::lookUp(const String& name) {
	lock();

	DistributedObject* obj = NULL;

	try {
		obj = namingDirectoryInterface->lookUp(name);
	} catch (...) {
		error("unreported Exception on lookUp()");
	}

	unlock();
	return obj;
}

DistributedObject* DistributedObjectBroker::lookUp(uint64 objid) {
	lock();

	DistributedObject* obj = NULL;

	try {
		obj = objectDirectory.get(objid);
	} catch (...) {
		error("unreported Exception on lookUp()");
	}

	unlock();

	return obj;
}

DistributedObjectStub* DistributedObjectBroker::undeploy(const String& name) {
	lock();

	DistributedObjectStub* obj = NULL;
	DistributedObjectServant* servant = NULL;

	try {
		obj = (DistributedObjectStub*) namingDirectoryInterface->undeploy(name);

		if (obj != NULL) {
			DistributedObjectAdapter* adapter = objectDirectory.remove(obj->_getObjectID());

			if (adapter != NULL) {
				servant = adapter->getImplementation();

				delete adapter;
			}

			info("object \'" + obj->_getName() + "\' deployed");
		}
	} catch (...) {
		error("unreported Exception on undeploy()");
	}

	unlock();

	if (servant != NULL) {
		info("deleting servant \'" + name + "\'");

		delete servant;
	}

	return obj;
}

/*ORBObjectAdapter* ObjectRequestBroker::getObjectAdapter(const String& name) {
	lock();

	ORBObjectStub* obj = namingDirectoryInterface->lookUp(name);
	ORBObjectAdapter* adapter = objectDirectory.getAdapter(obj->getObjectID());

	unlock();
	return adapter;
}*/

DistributedObjectAdapter* DistributedObjectBroker::getObjectAdapter(uint64 oid) {
	lock();

	DistributedObjectAdapter* adapter = NULL;

	try {
		adapter = objectDirectory.getAdapter(oid);
	} catch (...) {
		error("unreported Exception on getObjectAdapter()");
	}

	unlock();
	return adapter;
}
