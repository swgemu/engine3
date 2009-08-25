/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/service/proto/BasePacketHandler.h"

#include "DistributedObjectBroker.h"

#include "DistributedObjectBrokerClient.h"

#include "NamingDirectoryServiceImpl.h"

#include "DOBObjectManager.h"
#include "DOBObjectManagerImplementation.h"

DistributedObjectBroker::DistributedObjectBroker()
		: StreamServiceThread("DistributedObjectBroker") {
	phandler = NULL;

	namingDirectoryInterface = NULL;

	objectManager = NULL;

	orbClient = NULL;

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

	if (objectManager != NULL) {
		delete objectManager;
		objectManager = NULL;
	}
}

DistributedObjectBroker* DistributedObjectBroker::initialize(const String& addr, int port) {
	DistributedObjectBroker* inst = DistributedObjectBroker::instance();
	inst->address = addr;

	inst->initialize();

	if (addr.isEmpty())
		inst->start(port);

	return inst;
}

void DistributedObjectBroker::initialize() {
	if (address.isEmpty()) {
		namingDirectoryInterface = new NamingDirectoryServiceImpl();

		info("root naming directory initialized", true);
	} else {
		namingDirectoryInterface = new NamingDirectoryService(address);
		orbClient = namingDirectoryInterface->getClient();
	}

	phandler = new DOBPacketHandler("ORBPacketHandler", this);
	phandler->setLogging(false);

	if (address.isEmpty()) {
		objectManager = new DOBObjectManagerImplementation();
	} else {
		objectManager = new DOBObjectManager(orbClient);
	}
}

void DistributedObjectBroker::run() {
	acceptConnections();
}

void DistributedObjectBroker::shutdown() {
	stop();
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
		uint64 objectid = obj->_getObjectID();

		if (objectid == 0) {
			objectid = objectManager->getNextFreeObjectID();
			obj->_setObjectID(objectid);
		}

		namingDirectoryInterface->deploy(obj);

		if (objectManager->addObject(obj) != NULL) {
			StringBuffer msg;
			msg << "obejctid 0x" << hex << objectid << " already deployed";
			error(msg.toString());
			StackTrace::printStackTrace();
		} else
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
		uint64 objectid = obj->_getObjectID();

		if (objectid == 0) {
			objectid = objectManager->getNextFreeObjectID();
			obj->_setObjectID(objectid);
		}

		namingDirectoryInterface->deploy(name, obj);

		if (objectManager->addObject(obj) != NULL) {
			StringBuffer msg;
			msg << "obejctid 0x" << hex << objectid << " already deployed";
			error(msg.toString());
			StackTrace::printStackTrace();
		} else
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

		obj = objectManager->getObject(objid);

	} catch (Exception& e) {
		error(e.getMessage());
		e.printStackTrace();
	} catch (...) {
		error("unreported Exception on lookUp()");
	}

	unlock();

	if (obj == NULL)
		obj = objectManager->loadPersistentObject(objid);

	return obj;
}

DistributedObjectStub* DistributedObjectBroker::undeploy(const String& name) {
	lock();

	DistributedObjectStub* obj = NULL;
	DistributedObjectServant* servant = NULL;

	try {
		obj = (DistributedObjectStub*) namingDirectoryInterface->undeploy(name);

		if (obj != NULL) {
			DistributedObjectAdapter* adapter = objectManager->removeObject(obj->_getObjectID());

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

void DistributedObjectBroker::setCustomObjectManager(DOBObjectManager* manager) {
	lock();

	delete objectManager;
	objectManager = manager;

	unlock();
}

DistributedObjectAdapter* DistributedObjectBroker::getObjectAdapter(uint64 oid) {
	lock();

	DistributedObjectAdapter* adapter = NULL;

	try {
		adapter = objectManager->getAdapter(oid);
	} catch (...) {
		error("unreported Exception on getObjectAdapter()");
	}

	unlock();
	return adapter;
}
