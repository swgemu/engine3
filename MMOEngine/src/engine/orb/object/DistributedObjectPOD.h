/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/**
 * @author      : theanswer (theanswer@Victors-MacBook-Pro.local)
 * @file        : DistributedObjectPOD
 * @created     : Friday Mar 15, 2019 09:52:19 CET
 */

#ifndef DISTRIBUTEDOBJECTPOD_H

#define DISTRIBUTEDOBJECTPOD_H

#include "system/platform.h"
#include "system/lang/Object.h"

#include "engine/util/json.hpp"

namespace engine {
  namespace ORB {


	class DistributedObjectPOD : public Object {

	public:
		DistributedObjectPOD() {
		}

		virtual ~DistributedObjectPOD() {
		}

		virtual void writeJSON(nlohmann::json& j) {
		}

		virtual void readObject(ObjectInputStream* stream) = 0;
		virtual void writeObject(ObjectOutputStream* stream) = 0;
		virtual void writeObjectCompact(ObjectOutputStream* stream) = 0;

		uint64 _getObjectID() const { //for compat reasons
			return 0;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /* end of include guard DISTRIBUTEDOBJECTPOD_H */

