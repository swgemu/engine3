/*
 * SynchronizedHashTable.h
 *
 *  Created on: 22/12/2013
 *      Author: victor
 */

#ifndef SYNCHRONIZEDHASHTABLE_H_
#define SYNCHRONIZEDHASHTABLE_H_

#include "HashTable.h"

namespace sys {
  namespace util {

  	  template<class K, class V>
  	  class SynchronizedHashTable : public ReadWriteLock, public Object {
  		 HashTable<K, V> hashTable;
  	  public:
  		SynchronizedHashTable() : hashTable() {

  		}

  		SynchronizedHashTable(int initialCapacity) : hashTable(initialCapacity) {

  		}

  		SynchronizedHashTable(int initialCapacity, float loadFactor) : hashTable(initialCapacity, loadFactor) {

  		}

  		SynchronizedHashTable(const SynchronizedHashTable<K, V>& table) : Object(), hashTable(table.hashTable) {

  		}

  		V getNullValue() {
  			return hashTable.getNullValue();
  		}

  		V put(const K& key, const V& value) {
  			Locker locker(this);

  			V obj = hashTable.put(key, value);

  			return obj;
  		}

  		V get(const K& key) {
  			ReadLocker locker(this);

  			V obj = hashTable.get(key);

  			return obj;
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

  		Entry<K, V>* getEntryUnsafe(const K& key) {
  			return hashTable.getEntry(key);
  		}

  		bool containsKey(const K& key) {
  			ReadLocker locker(this);

  			bool val = hashTable.containsKey(key);

  			return val;
  		}

  		void copyFrom(HashTable<K, V>* htable) {
  			Locker locker(this);

  			hashTable.copyFrom(htable);
  		}

  		SynchronizedHashTable<K, V>& operator=(const HashTable<K, V>& htable) {
  			Locker locker(this);

  			hashTable.operator=(htable);

  			return *this;
  		}

  		V remove(const K& key) {
  			Locker locker(this);

  			V obj = hashTable.remove(key);

  			return obj;
  		}

  		void removeAll() {
  			Locker locker(this);

  			hashTable.removeAll();
  		}

  		int size() {
  			return hashTable.size();
  		}
  	  };
  }
}

using namespace sys::util;

#endif /* SYNCHRONIZEDHASHTABLE_H_ */
