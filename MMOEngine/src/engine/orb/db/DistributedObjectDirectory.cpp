/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "DistributedObjectDirectory.h"

#include "engine/db/ObjectDatabaseManager.h"

#include "engine/core/ManagedObject.h"
#include <chrono>

class DirectoryKeyHandler : public KeyHandler<uint64> {
	AtomicLong id;

public:
	uint64 generateKey() {
		return id.increment();
	}
};

namespace DOBT {
	int hash(const uint64& keyValue) {
		uint64 key = keyValue;

		key = (~key) + (key << 18); // key = (key << 18) - key - 1;
		key =   key  ^ (key >> 31);
		key = key * 21;             // key = (key + (key << 2)) + (key << 4);
		key = key ^ (key >> 11);
		key = key + (key << 6);
		key = key ^ (key >> 22);

		return (int) key;
	}
}

EngineObjectHashTable::EngineObjectHashTable() : HashTable<uint64, DistributedObjectAdapter*>() {
}

EngineObjectHashTable::EngineObjectHashTable(int initialCapacity) : HashTable<uint64, DistributedObjectAdapter*>(initialCapacity) {
}

int EngineObjectHashTable::hash(const uint64& keyValue) const { //actual uint64 hash function instead of the default PoS AtomicLong has
	return DOBT::hash(keyValue);
}

ObjectHashTableHelper::ObjectHashTableHelper() : HashTable<uint64, DistributedObject*>() {
}

ObjectHashTableHelper::ObjectHashTableHelper(int initialCapacity) : HashTable<uint64, DistributedObject*>(initialCapacity) {
}

int ObjectHashTableHelper::hash(const uint64& keyValue) const { //actual uint64 hash function instead of the default PoS AtomicLong has
	return DOBT::hash(keyValue);
}

DistributedObjectDirectory::DistributedObjectDirectory() : objectMap(300000) {
}

DistributedObjectDirectory::~DistributedObjectDirectory() {
	HashTableIterator<uint64, DistributedObjectAdapter*> iterator = objectMap.iterator();

	while (iterator.hasNext()) {
		delete iterator.getNextValue();
	}
}

DistributedObjectAdapter* DistributedObjectDirectory::add(uint64 objid, DistributedObjectAdapter* adapter) {
	helperObjectMap.add(objid, adapter->getStub());

	return objectMap.put(objid, adapter);
}

DistributedObject* DistributedObjectDirectory::get(uint64 objid) {
	DistributedObjectAdapter* adapter = nullptr;

	adapter = objectMap.get(objid);

	if (adapter != nullptr) {
		if (!helperObjectMap.contains(objid)) {
			helperObjectMap.add(objid, adapter->getStub());
		}

		return adapter->getStub();
	} else
		return nullptr;
}

DistributedObjectAdapter* DistributedObjectDirectory::remove(uint64 objid) {
	DistributedObjectAdapter* adapter = objectMap.get(objid);

	if (adapter != nullptr)
		objectMap.remove(objid);

	helperObjectMap.remove(objid);

	return adapter;
}

void DistributedObjectDirectory::removeHelper(uint64 objid) {
	helperObjectMap.remove(objid);
}

bool DistributedObjectDirectory::tryRemoveHelper(sys::uint64 objid) {
	return helperObjectMap.tryRemoveAndDestroy(objid);
}

DistributedObjectAdapter* DistributedObjectDirectory::getAdapter(uint64 objid) {
	return objectMap.get(objid);
}

