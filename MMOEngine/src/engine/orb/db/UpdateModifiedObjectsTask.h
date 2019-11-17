/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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
				DistributedObjectBroker::instance()->getObjectManager()->createBackup(false);
		}
	};

  } // namespace ORB
} // namespace engine

#endif /* UPDATEMODIFIEDOBJECTSTASK_H_ */
