/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef UPDATEMODIFIEDOBJECTSTASK_H_
#define UPDATEMODIFIEDOBJECTSTASK_H_

#include "engine/orb/DistributedObjectBroker.h"

#include "engine/orb/db/DOBObjectManager.h"

namespace engine {
	namespace ORB {

	class UpdateModifiedObjectsTask : public Task {
	public:
		UpdateModifiedObjectsTask() {
		}

		void run() {
			if (DistributedObjectBroker::instance()->isRootBroker())
				DistributedObjectBroker::instance()->getObjectManager()->createBackup();
		}
	};

  } // namespace ORB
} // namespace engine

#endif /* UPDATEMODIFIEDOBJECTSTASK_H_ */
