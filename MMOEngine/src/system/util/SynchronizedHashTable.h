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
  	  class SynchronizedHashTable : public HashTable<K, V>, public ReadWriteLock {
  	  public:
  		SynchronizedHashTable() : HashTable<K, V>() {

  		}

  		SynchronizedHashTable(int initialCapacity) : HashTable<K, V>(initialCapacity) {

  		}

  		SynchronizedHashTable(int initialCapacity, float loadFactor) : HashTable<K, V>(initialCapacity, loadFactor) {

  		}

  		SynchronizedHashTable(const SynchronizedHashTable<K, V>& table) : HashTable<K, V>(table) {

  		}

  		V& put(const K& key, const V& value) {
  			Locker locker(this);

  			return HashTable<K, V>::put(key, value);
  		}

  		V& get(const K& key) {
  			ReadLocker locker(this);

  			return HashTable<K, V>::get(key);
  		}

  		Entry<K, V>* getEntry(const K& key) {
  			ReadLocker locker(this);

  			return HashTable<K, V>::getEntry(key);
  		}

  		bool containsKey(const K& key) {
  			ReadLocker locker(this);

  			return HashTable<K, V>::containsKey(key);
  		}

  		void copyFrom(HashTable<K, V>* htable) {
  			Locker locker(this);

  			HashTable<K, V>::copyFrom(htable);
  		}

  		SynchronizedHashTable<K, V>& operator=(const HashTable<K, V>& htable) {
  			Locker locker(this);

  			HashTable<K, V>::operator=(htable);

  			return *this;
  		}

  		V remove(const K& key) {
  			Locker locker(this);

  			return HashTable<K, V>::remove(key);
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
