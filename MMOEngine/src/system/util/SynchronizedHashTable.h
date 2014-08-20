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

  		SynchronizedHashTable(const SynchronizedHashTable<K, V>& table) : hashTable(table) {

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

  		bool toBinaryStream(ObjectOutputStream* stream) {
  			return hashTable.toBinaryStream(stream);
  		}

  		bool parseFromBinaryStream(ObjectInputStream* stream) {
  			return hashTable.parseFromBinaryStream(stream);
  		}

  		Entry<K, V>* getEntry(const K& key) {
  			ReadLocker locker(this);

  			Entry<K, V>* entry = HashTable<K, V>::getEntry(key);

  			return entry;
  		}

  		bool containsKey(const K& key) {
  			ReadLocker locker(this);

  			bool val = HashTable<K, V>::containsKey(key);

  			return val;
  		}

  		void copyFrom(HashTable<K, V>* htable) {
  			Locker locker(this);

  			HashTable<K, V>::copyFrom(htable);
  		}

  		SynchronizedHashTable<K, V>& operator=(const HashTable<K, V>& htable) {
  			Locker locker(this);

  			hashTable.operator=(htable);

  			return *this;
  		}

  		V remove(const K& key) {
  			Locker locker(this);

  			V obj = HashTable<K, V>::remove(key);

  			return obj;
  		}

  		void removeAll() {
  			Locker locker(this);

  			HashTable<K, V>::removeAll();
  		}
  	  };
  }
}

using namespace sys::util;

#endif /* SYNCHRONIZEDHASHTABLE_H_ */
