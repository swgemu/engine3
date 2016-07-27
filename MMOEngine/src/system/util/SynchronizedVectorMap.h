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
 	 class SynchronizedVectorMap : public Object {
 		 VectorMap<K, V> vectorMap;

 		 mutable ReadWriteLock guard;
 	 public:
 		 SynchronizedVectorMap();
 		 SynchronizedVectorMap(int initsize, int incr);
 		 SynchronizedVectorMap(const SynchronizedVectorMap<K, V>& vector);

 		 SynchronizedVectorMap<K, V>& operator=(const SynchronizedVectorMap<K, V>& vector);

 		 int put(const K& key, const V& value);

 		 V get(int index) const;
 		 V get(const K& key) const;

		 K getKey(int index) const;

 		 int find(const K& key) const;

 		 bool toBinaryStream(ObjectOutputStream* stream);

 		 bool parseFromBinaryStream(ObjectInputStream* stream);

 		 Object* clone();

 		 bool contains(const K& key) const;

 		 bool drop(const K& key);

 		 int size() const {
 			 return vectorMap.size();
 		 }

 		 inline void setInsertPlan(int plan) {
 			 vectorMap.setInsertPlan(plan);
 		 }

 		 inline void setNoDuplicateInsertPlan() {
 			 vectorMap.setNoDuplicateInsertPlan();
 		 }

 		 inline void setAllowDuplicateInsertPlan() {
 			 vectorMap.setAllowDuplicateInsertPlan();
 		 }

 		 inline void setAllowOverwriteInsertPlan() {
 			 vectorMap.setAllowOverwriteInsertPlan();
 		 }

 		 inline void setNullValue(V val) {
 			 vectorMap.setNullValue(val);
 		 }

 		 inline int getInsertPlan() const {
 			 return vectorMap.getInsertPlan();
 		 }
 	 };

 	 template<class K, class V> SynchronizedVectorMap<K, V>::SynchronizedVectorMap()
		: vectorMap() {
 	 }

 	 template<class K, class V> SynchronizedVectorMap<K, V>::SynchronizedVectorMap(int initsize, int incr)
		: vectorMap(initsize, incr) {
 	 }

 	 template<class K, class V> SynchronizedVectorMap<K, V>::SynchronizedVectorMap(const SynchronizedVectorMap<K, V>& vector)
		: Object(), vectorMap(vector.vectorMap) {
 	 }

 	 template<class K, class V> SynchronizedVectorMap<K, V>& SynchronizedVectorMap<K, V>::operator=(const SynchronizedVectorMap<K, V>& vector) {
 		 Locker locker(&guard);

 		 if (this == &vector)
 			 return *this;

 		 vectorMap.operator=(vector);

 		 return *this;
 	 }

 	 template<class K, class V> bool SynchronizedVectorMap<K, V>::toBinaryStream(ObjectOutputStream* stream) {
 		 return vectorMap.toBinaryStream(stream);
 	 }

 	 template<class K, class V> bool SynchronizedVectorMap<K, V>::parseFromBinaryStream(ObjectInputStream* stream) {
 		 return vectorMap.parseFromBinaryStream(stream);
 	 }

 	 template<class K, class V> Object* SynchronizedVectorMap<K, V>::clone() {
 		 ReadLocker locker(&guard);

 		 SynchronizedVectorMap<K, V>* val = new SynchronizedVectorMap<K, V>(*this);

 		 return val;
 	 }

 	 template<class K, class V> int SynchronizedVectorMap<K, V>::put(const K& key, const V& value) {
 		 Locker locker(&guard);

 		 int ret = vectorMap.put(key, value);

 		 return ret;
 	 }

 	 template<class K, class V> V SynchronizedVectorMap<K, V>::get(int index) const {
 		 ReadLocker locker(&guard);

 		 V obj = const_cast<VectorMap<K,V>&>(vectorMap).get(index);

 		 return obj;
 	 }

	 template<class K, class V> K SynchronizedVectorMap<K, V>::getKey(int index) const {
		 ReadLocker locker(&guard);

		 return vectorMap.elementAt(index).getKey();
	 }

 	 template<class K, class V> V SynchronizedVectorMap<K, V>::get(const K& key) const {
 		 ReadLocker locker(&guard);

 		 V obj = const_cast<VectorMap<K,V>&>(vectorMap).get(key);

 		 return obj;
 	 }

 	 template<class K, class V> int SynchronizedVectorMap<K, V>::find(const K& key) const {
 		 ReadLocker locker(&guard);

 		 int ret = vectorMap.find(key);

 		 return ret;
 	 }

 	 template<class K, class V> bool SynchronizedVectorMap<K, V>::contains(const K& key) const {
 		ReadLocker locker(&guard);

 		bool ret = vectorMap.contains(key);

 		return ret;
 	 }

 	 template<class K, class V> bool SynchronizedVectorMap<K, V>::drop(const K& key) {
 		 Locker locker(&guard);

 		 bool val = vectorMap.drop(key);

 		 return val;
 	 }
 }
}

#endif /* SYNCHRONIZEDVECTORMAP_H_ */
