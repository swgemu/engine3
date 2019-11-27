/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/core/Core.h"

#include "DistributedObjectBroker.h"

#include "control/ObjectBrokerDirector.h"
#include "control/ObjectBrokerAgent.h"

#include "messages/DOBServiceHandler.h"

#include "db/DOBObjectManager.h"

DistributedObjectBroker::DistributedObjectBroker()
		: StreamServiceThread("DistributedObjectBroker") {
	rootObjectBroker = nullptr;

	namingDirectoryService = nullptr;

	objectManager = nullptr;

	port = 0;

	//setDebugLogLevel();
	setInfoLogLevel();
}

DistributedObjectBroker::~DistributedObjectBroker() {
	shutdown();

	if (namingDirectoryService != nullptr) {
		delete namingDirectoryService;
		namingDirectoryService = nullptr;
	}

	if (rootObjectBroker != nullptr) {
		delete rootObjectBroker;
		rootObjectBroker = nullptr;
	}

	if (serviceHandler != nullptr) {
		delete serviceHandler;
		serviceHandler = nullptr;
	}
}

DistributedObjectBroker* DistributedObjectBroker::initialize(const String& addr, int port) {
	DistributedObjectBroker* inst = DistributedObjectBroker::instance();
	inst->address = addr;
	inst->port = port;

	try {
		if (addr.isEmpty())
			inst->start(port);
	} catch (const Exception& e) {
		inst->address = "127.0.0.1";
	}

	inst->initialize();

	return inst;
}

void DistributedObjectBroker::initialize() {
	if (address.isEmpty()) {
		info("root naming directory initialized", true);

		ObjectBrokerDirector::instance()->start();
	} else {
		info("attaching to root naming directory at " + address + ":" + String::valueOf(port), true);

		rootObjectBroker = new RemoteObjectBroker(address, port);
	}

	ObjectBrokerDirector::instance()->brokerConnected(this);

	namingDirectoryService = new NamingDirectoryService();

	DOBServiceHandler* serviceHandler = new DOBServiceHandler();
	setHandler(serviceHandler);

	objectManager = nullptr; //new DOBObjectManager();

	ObjectBrokerAgent::instance()->start();
}

void DistributedObjectBroker::run() {
	acceptConnections();
}

void DistributedObjectBroker::shutdown() {
	if (socket != nullptr) {
		StreamServiceThread::stop();

		delete socket;
		socket = nullptr;
	}
}

void DistributedObjectBroker::registerClass(const String& name, DistributedObjectClassHelper* helper) {
	//Locker locker(this);

	classMap.put(name, helper);
}

void DistributedObjectBroker::deploy(DistributedObjectStub* obj) {
	uint64 objectid = obj->_getObjectID();
	const String& name = obj->_getName();

	if (!isRootBroker()) {
		debug() << "deploying object \"" << name << "\" 0x" << hex << objectid << " remotely";

		rootObjectBroker->deploy(obj);
	}

	deployLocal(name, obj);
}

void DistributedObjectBroker::deploy(const String& name, DistributedObjectStub* obj) {
	if (!isRootBroker()) {
		debug() << "deploying object \"" << name << "\" remotely";

		rootObjectBroker->deploy(name, obj);
	}

	deployLocal(name, obj);
}

Reference<DistributedObject*> DistributedObjectBroker::lookUp(const String& name) {
	Locker locker(objectManager);

	Reference<DistributedObject*> object = namingDirectoryService->lookup(name);
	if (object != nullptr)
		return object;

	locker.release();

	if (!isRootBroker()) {
		debug() << "looking up object \"" << name << "\" remotely";

		object = rootObjectBroker->lookUp(name);
	}

	return object;
}

void DistributedObjectBroker::requestServant(DistributedObjectStub* obj) {
	if (!isRootBroker()) {
		rootObjectBroker->requestServant(obj);
	}
}

Reference<DistributedObject*> DistributedObjectBroker::lookUp(uint64 objid) {
	//Locker locker(objectManager);

	Reference<DistributedObject*> object = nullptr;

	object = objectManager->getObject(objid);

	//locker.release();

	if (object == nullptr)
		object = objectManager->loadPersistentObject(objid);

	if (object != nullptr)
		return object;

	if (!isRootBroker()) {
		object = remoteObjectCache.get(objid);
		if (object != nullptr) {
			debug() << "found object 0x" << hex << objid << " in remote cache";

			return object;
		}

		debug() << "looking up object 0x" << hex << objid << " remotely";

		return rootObjectBroker->lookUp(objid);
	}

	return object;
}

bool DistributedObjectBroker::destroyObject(DistributedObjectStub* obj) {
	if (objectManager == nullptr) {
		//obj->_setDestroying();

		obj->undeploy();

		return true;
	}

	Locker clocker(objectManager);

	/*
	if (obj->_isGettingDestroyed())
		return false;

	if (obj->getReferenceCount() % 2)
		return false;
	 */

	//obj->_setDestroying();

	obj->undeploy();

	return true;
}

DistributedObjectStub* DistributedObjectBroker::undeploy(const String& name) {
	if (!isRootBroker()) {
		debug() << "undeploying object \"" << name << "\" remotely";

		rootObjectBroker->undeploy(name);
	}

	return undeployLocal(name);
}

uint64 DistributedObjectBroker::getNextFreeObjectID() {
	if (!isRootBroker()) {
		debug("getting next free objectID remotely");

		return rootObjectBroker->getNextFreeObjectID();
	}

	return objectManager->getNextFreeObjectID();
}

void DistributedObjectBroker::deployLocal(const String& name, DistributedObjectStub* obj) {
	if (objectManager == nullptr)
		return;

	Locker locker(objectManager);

#ifndef WITH_STM
	fatal(!obj->isDeployed()) << "obj is deployed";
#endif

	objectManager->createObjectID(name, obj);

	if (!namingDirectoryService->bind(obj->_getName(), obj))
		throw NameAlreadyBoundException(obj);

	if (objectManager->addObject(obj) != nullptr) {
		throw ObjectAlreadyDeployedException(obj);
	} else
		debug() << "object \'" << obj->_getName() << "\' deployed with ID 0x" << hex << ((int64)obj->_getObjectID());

	obj->setDeployed(true);
}

void DistributedObjectBroker::deployRemote(const String& name, DistributedObjectStub* obj) {
	Locker locker(objectManager);

	fatal(!obj->isDeployed()) << "obj is deployed";

	objectManager->createObjectID(name, obj);

	if (!namingDirectoryService->bind(obj->_getName(), obj))
		throw NameAlreadyBoundException(obj);

	if (remoteObjectCache.add(obj->_getObjectID(), obj)) {
		throw ObjectAlreadyDeployedException(obj);
	} else
		debug() << "remote object \'" << obj->_getName() <<
			"\' deployed with ID 0x" << hex << ((int64)obj->_getObjectID());

	obj->setDeployed(true);
}

DistributedObjectStub* DistributedObjectBroker::undeployLocal(const String& name) {
	if (objectManager == nullptr)
		return nullptr;

	Locker locker(objectManager); //Locker locker(this);

	DistributedObjectStub* obj = (DistributedObjectStub*) namingDirectoryService->unbind(name);

	locker.release();

	if (obj != nullptr) {
		DistributedObjectAdapter* adapter = objectManager->removeObject(obj->_getObjectID());

		if (adapter != nullptr) {
			delete adapter;
		}

	#ifndef WITH_STM
		obj->_setImplementation(nullptr);

		/*if (servant != nullptr) {
			debug("deleting servant \'" + name + "\'");

			delete servant;
		}*/
	#endif

		debug() << "object \'" << obj->_getName() << "\' undeployed";
	}

	return obj;
}

DistributedObjectStub* DistributedObjectBroker::undeployRemote(const String& name) {
	Locker locker(objectManager); //Locker locker(this);

	DistributedObjectStub* obj = (DistributedObjectStub*) namingDirectoryService->unbind(name);

	locker.release();

	if (obj != nullptr) {
		remoteObjectCache.remove(obj->_getObjectID());

		debug() << "remote object \'" << obj->_getName() << "\' undeployed";
	}

	return obj;
}

void DistributedObjectBroker::setCustomObjectManager(DOBObjectManager* manager) {
	Locker locker(this);

	delete objectManager;
	objectManager = manager;
}

DistributedObjectStub* DistributedObjectBroker::createObjectStub(const String& className, const String& name) {
	DistributedObjectStub* obj = nullptr;

	DistributedObjectClassHelper* helper = classMap.get(className);
	if (helper != nullptr) {
		debug() << "class \'" << className << "\' found when creating stub";

		obj = dynamic_cast<DistributedObjectStub*>(helper->instantiateObject());

		obj->_setName(name);

		obj->_setObjectBroker(this);
	} else
		warning() << "class \'" << className << "\' is not declared when creating stub";

	return obj;
}


DistributedObjectPOD* DistributedObjectBroker::createObjectPOD(const String& className) {
	DistributedObjectPOD* obj = nullptr;

	DistributedObjectClassHelper* helper = classMap.get(className);
	if (helper != nullptr) {
		return helper->instantiatePOD();
	}

	return obj;
}

DistributedObjectServant* DistributedObjectBroker::createObjectServant(const String& className, DistributedObjectStub* stub, bool addToObjectManager) {
	DistributedObjectServant* servant = nullptr;

	DistributedObjectClassHelper* helper = classMap.get(className);
	if (helper != nullptr) {
		debug() << "class \'" << className << "\' found when creating servant";

		servant = helper->instantiateServant();

		servant->_setStub(stub);
		servant->_setClassHelper(helper);

		stub->_setImplementation(servant);

		if (addToObjectManager) {
			objectManager->addObject(stub);
		}
	} else
		warning("class \'" + className + "\' is not declared when creating servant");


	return servant;
}

DistributedObjectAdapter* DistributedObjectBroker::getObjectAdapter(uint64 oid) {
	Locker locker(objectManager);//Locker locker(this);

	return objectManager->getAdapter(oid);
}

