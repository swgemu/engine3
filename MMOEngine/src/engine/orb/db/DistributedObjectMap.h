/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DISTRIBUTEDOBJECTMAP_H_
#define DISTRIBUTEDOBJECTMAP_H_

#include "engine/orb/object/DistributedObject.h"

namespace engine {
  namespace ORB {

	template<class ObjectMap>
	class DistributedObjectMap {
		ObjectMap objectMap;

	public:
		DistributedObjectMap() {
		}

		~DistributedObjectMap() {
			auto iterator = objectMap.iterator();

			while (iterator.hasNext()) {
				auto ref = iterator.getNextValue();

				ref->release();
			}
		}

		bool add(sys::uint64 objectID, DistributedObject* object) {
			bool exists = objectMap.put(objectID, object) != nullptr;

			if (!exists) {
				object->acquire();
			}

			return exists;
		}

		DistributedObject* get(sys::uint64 objectID) {
			return objectMap.get(objectID);
		}

		bool contains(sys::uint64 objectID) {
			return objectMap.containsKey(objectID);
		}

		bool tryRemoveAndDestroy(sys::uint64 objectID) {
			DistributedObject* obj = objectMap.remove(objectID);

			if (obj) {
				bool res = obj->tryFinalRelease();

				if (!res) {
					objectMap.put(objectID, obj);

					return false;
				} else {
					return true;
				}
			}

			return false;
		}

		void remove(sys::uint64 objectID) {
			DistributedObject* obj = objectMap.remove(objectID);

			if (obj) {
				obj->release();
			}

			//return false;
		}

		inline int getSize() const {
			return objectMap.size();
		}

		ObjectMap* getMap() {
			return &objectMap;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTMAP_H_*/
