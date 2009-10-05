/*
 * ObjectSaveToDatabaseTask.h
 *
 *  Created on: 16/08/2009
 *      Author: victor
 */

#ifndef OBJECTSAVETODATABASETASK_H_
#define OBJECTSAVETODATABASETASK_H_

#include "engine/engine.h"

#include "ManagedObject.h"

namespace engine {
  namespace core {

	class ObjectUpdateToDatabaseTask : public Task {
		ManagedReference<ManagedObject*> object;

	public:
		ObjectUpdateToDatabaseTask(ManagedObject* object) : Task(300000) {
			ObjectUpdateToDatabaseTask::object = object;
		}

		void run() {
			try {
				object->wlock();

				object->clearUpdateToDatabaseTask();
				object->updateToDatabase();

				object->unlock();
			} catch (Exception& e) {
				//object->error(e.getMessage());
				object->unlock();
			} catch (...) {
				//object->error("unreported exception caught in ObjectSaveToDatabaseTask::run");
				object->unlock();
			}
		}
	};

  } // namespace core
} // namespace engine

#endif /* OBJECTSAVETODATABASETASK_H_ */
