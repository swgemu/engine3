/*
 * SynchronizedVectorMap.h
 *
 *  Created on: 20/12/2013
 *      Author: victor
 */

#ifndef SYNCHRONIZEDVECTORMAP_H_
#define SYNCHRONIZEDVECTORMAP_H_

#include "VectorMap.h"

namespace sys {
 namespace util {
 	 template<class K, class V> class VectorMap;

 	 template<class K, class V>
 	 class SynchronizedVectorMap : public VectorMap<K, V> {
 		 mutable ReadWriteLock guard;
 	 public:
 		 SynchronizedVectorMap();
 		 SynchronizedVectorMap(int initsize, int incr);
 		 SynchronizedVectorMap(const SynchronizedVectorMap<K, V>& vector);

 		 SynchronizedVectorMap<K, V>& operator=(const SynchronizedVectorMap<K, V>& vector);

 		 int put(const K& key, const V& value);

 		 V& get(int index);
 		 V& get(const K& key);

 		 int find(const K& key);

 		 Object* clone();

 		 bool contains(const K& key);

 		 bool drop(const K& key);
 	 };

 	 template<class K, class V> SynchronizedVectorMap<K, V>::SynchronizedVectorMap()
		: VectorMap<K, V>() {
 	 }

 	 template<class K, class V> SynchronizedVectorMap<K, V>::SynchronizedVectorMap(int initsize, int incr)
		: VectorMap<K, V>(initsize, incr) {
 	 }

 	 template<class K, class V> SynchronizedVectorMap<K, V>::SynchronizedVectorMap(const SynchronizedVectorMap<K, V>& vector)
		: VectorMap<K, V>(vector) {
 	 }

 	 template<class K, class V> SynchronizedVectorMap<K, V>& SynchronizedVectorMap<K, V>::operator=(const SynchronizedVectorMap<K, V>& vector) {
 		 Locker locker(&guard);

 		 if (this == &vector)
 			 return *this;

 		 VectorMap<K, V>::operator=(vector);

 		 return *this;
 	 }

 	 template<class K, class V> Object* SynchronizedVectorMap<K, V>::clone() {
 		 ReadLocker locker(&guard);

 		 return new SynchronizedVectorMap<K, V>(*this);
 	 }

 	 template<class K, class V> int SynchronizedVectorMap<K, V>::put(const K& key, const V& value) {
 		 Locker locker(&guard);

 		 return VectorMap<K, V>::put(key, value);
 	 }

 	 template<class K, class V> V& SynchronizedVectorMap<K, V>::get(int index) {
 		 ReadLocker locker(&guard);

 		 return VectorMap<K, V>::get(index);
 	 }

 	 template<class K, class V> V& SynchronizedVectorMap<K, V>::get(const K& key) {
 		 ReadLocker locker(&guard);

 		 return VectorMap<K, V>::get(key);
 	 }

 	 template<class K, class V> int SynchronizedVectorMap<K, V>::find(const K& key) {
 		 ReadLocker locker(&guard);

 		 return VectorMap<K, V>::find(key);
 	 }

 	 template<class K, class V> bool SynchronizedVectorMap<K, V>::contains(const K& key) {
 		ReadLocker locker(&guard);

 		return VectorMap<K, V>::contains(key);
 	 }

 	 template<class K, class V> bool SynchronizedVectorMap<K, V>::drop(const K& key) {
 		 Locker locker(&guard);

 		 return VectorMap<K, V>::drop(key);
 	 }
 }
}

#endif /* SYNCHRONIZEDVECTORMAP_H_ */
