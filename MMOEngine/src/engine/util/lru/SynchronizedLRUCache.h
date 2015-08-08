/*
 * SynchronizedLRUCache.h
 *
 *  Created on: 08/08/2015
 *      Author: victor
 */

#ifndef SYNCHRONIZEDLRUCACHE_H_
#define SYNCHRONIZEDLRUCACHE_H_

#include "lru_cache_using_engine3.h"

template <typename K,typename V> class SynchronizedLRUCache :
		public lru_cache_using_engine3<K,V> {
public:
	SynchronizedLRUCache(V (*function)(const K&), size_t capacity, int minAccessCountForPromotion = 5)
		: lru_cache_using_engine3<K,V>(function, capacity, minAccessCountForPromotion) {
	}
};

#endif /* SYNCHRONIZEDLRUCACHE_H_ */
