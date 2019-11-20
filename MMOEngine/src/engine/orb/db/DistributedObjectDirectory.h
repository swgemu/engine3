/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DISTRIBUTEDOBJECTDIRECTORY_H_
#define DISTRIBUTEDOBJECTDIRECTORY_H_

#include "engine/orb/object/DistributedObject.h"
#include "engine/orb/object/DistributedObjectAdapter.h"

#include "DistributedObjectMap.h"

#include "engine/util/ObjectContainer.h"

#include "system/util/SynchronizedHashTable.h"
#include "engine/util/bytell_hash_map.hpp"

namespace engine {
  namespace ORB {

	class EngineObjectHashTable : public HashTable<uint64, DistributedObjectAdapter*> {
	public:
		EngineObjectHashTable();

		EngineObjectHashTable(int initialCapacity);

		int hash(const uint64& keyValue) const override;
	};

	class ObjectHashTableHelper : public HashTable<uint64, DistributedObject*> {
	public:
		ObjectHashTableHelper();

		ObjectHashTableHelper(int initialCapacity);

		int hash(const uint64& keyValue) const override;
	};

	class DistributedObjectDirectory {
	protected:
		using ObjectHashTable = EngineObjectHashTable;

		ObjectHashTable objectMap;

		DistributedObjectMap<SynchronizedHashTable<uint64, DistributedObject*,
			ObjectHashTableHelper>> helperObjectMap;

	public:
		DistributedObjectDirectory();
		~DistributedObjectDirectory();

		DistributedObjectAdapter* add(sys::uint64 objid, DistributedObjectAdapter* adapter);

		DistributedObject* get(sys::uint64 objid);

		DistributedObjectAdapter* remove(sys::uint64 objid);

		void removeHelper(sys::uint64 objid);

		bool tryRemoveHelper(sys::uint64 objid);

		DistributedObjectAdapter* getAdapter(uint64 objid);

		inline int getSize() const {
			return objectMap.size();
		}

		const ObjectHashTable& getObjectHashTable() const {
			return objectMap;
		}

		ObjectHashTableHelper* getDistributedObjectMap() {
			return helperObjectMap.getMap()->getHashTable();
		}
	};

  } // namespace ORB
} // namespace engine


using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTDIRECTORY_H_*/
