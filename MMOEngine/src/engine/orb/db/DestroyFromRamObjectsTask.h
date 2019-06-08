/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * DestroyFromRamObjectsTask.h
 *
 *  Created on: 10/10/2012
 *      Author: victor
 */

#ifndef DESTROYFROMRAMOBJECTSTASK_H_
#define DESTROYFROMRAMOBJECTSTASK_H_

class DestroyFromRamObjectsTask : public Task {
	Vector<DistributedObject*>* objects;

public:
	DestroyFromRamObjectsTask(Vector<DistributedObject*>* obj) {
		objects = obj;
	}

	void run() {
		Logger::console.info("starting to delete " + String::valueOf(objects->size()) + " objects from ram...", true);

		for (int i = 0; i < objects->size(); ++i) {
			objects->get(i)->_destroyIgnoringCount();
		}

		delete objects;
		objects = nullptr;

		Logger::console.info("finished deleting objects from ram", true);
	}
};


#endif /* DESTROYFROMRAMOBJECTSTASK_H_ */
