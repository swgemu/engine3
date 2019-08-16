/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * SynchronizedHashTable.h
 *
 *  Created on: 22/12/2013
 *      Author: victor
 */

#ifndef SYNCHRONIZEDHASHTABLE_H_
#define SYNCHRONIZEDHASHTABLE_H_

#include "HashTable.h"

#include "system/thread/ReadLocker.h"
#include "system/thread/Locker.h"

namespace sys {
  namespace util {

  	  template<class K, class V, class Table = HashTable<K, V> >
  	  class SynchronizedHashTable : public ReadWriteLock, public Object {
		using this_type = sys::util::SynchronizedHashTable<K, V, Table>;

	  protected:
  		Table hashTable;

  	  public:
  		SynchronizedHashTable() : hashTable() {

  		}

  		SynchronizedHashTable(int initialCapacity) : hashTable(initialCapacity) {

  		}

  		SynchronizedHashTable(int initialCapacity, float loadFactor) : hashTable(initialCapacity, loadFactor) {

  		}

  		SynchronizedHashTable(const SynchronizedHashTable<K, V, Table>& table) : Object(), hashTable(table.hashTable) {

  		}

  		V getNullValue() {
  			return hashTable.getNullValue();
  		}

  		V put(const K& key, const V& value) {
  			Locker locker(this);

  			return hashTable.put(key, value);
  		}

  		V get(const K& key) const {
  			ReadLocker locker(this);

  			return hashTable.get(key);
  		}

  		const V& getUnsafe(const K& key) const {
  			return hashTable.get(key);
  		}

		V& getUnsafe(const K& key) {
  			return hashTable.get(key);
  		}

  		bool toBinaryStream(ObjectOutputStream* stream) {
  			return hashTable.toBinaryStream(stream);
  		}

  		bool parseFromBinaryStream(ObjectInputStream* stream) {
  			return hashTable.parseFromBinaryStream(stream);
  		}

  		const Entry<K, V>* getEntryUnsafe(const K& key) const {
  			return hashTable.getEntry(key);
  		}

		Entry<K, V>* getEntryUnsafe(const K& key) {
  			return hashTable.getEntry(key);
  		}

  		bool containsKey(const K& key) const {
			ReadLocker locker(this);

  			return hashTable.containsKey(key);
  		}

  		void copyFrom(Table* htable) {
  			Locker locker(this);

  			hashTable.copyFrom(htable);
  		}

  		SynchronizedHashTable<K, V, Table>& operator=(const Table& htable) {
  			Locker locker(this);

  			hashTable.operator=(htable);

  			return *this;
  		}

  		V remove(const K& key) {
  			Locker locker(this);

  			return hashTable.remove(key);
  		}

  		void removeAll() {
  			Locker locker(this);

  			hashTable.removeAll();
  		}

  		int size() const {
  			return hashTable.size();
  		}

		HashTableIterator<K, V> iterator() {
			return hashTable.iterator();
		};

		Table* getHashTable() {
			return &hashTable;
		}

		const Table* getHashTable() const {
			return &hashTable;
		}
  	  };
  }
}

using namespace sys::util;

#endif /* SYNCHRONIZEDHASHTABLE_H_ */
