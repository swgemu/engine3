/*
 * ObjectUpdateToDatabaseTask.cpp
 *
 *  Created on: 05/10/2009
 *      Author: victor
 */

#include "ObjectUpdateToDatabaseTask.h"
#include "ManagedObject.h"
#include "ManagedReference.h"

ObjectUpdateToDatabaseTask::ObjectUpdateToDatabaseTask(ManagedObject* object) : Task(300000) {
	ObjectUpdateToDatabaseTask::object = object;
}

void ObjectUpdateToDatabaseTask::run() {
	ManagedReference<ManagedObject*> strongRef = object.get();

	if (strongRef == NULL)
		return;

	try {
		strongRef->wlock();

		strongRef->clearUpdateToDatabaseTask();
		//strongRef->updateToDatabase();

		strongRef->unlock();
	} catch (Exception& e) {
		//object->error(e.getMessage());
		strongRef->unlock();
	} catch (...) {
		//object->error("unreported exception caught in ObjectSaveToDatabaseTask::run");
		strongRef->unlock();
	}
}
