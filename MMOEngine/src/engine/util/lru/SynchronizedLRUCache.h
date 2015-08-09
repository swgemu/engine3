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
	SynchronizedLRUCache(LRUFunction<K, V>* function, size_t capacity, int minAccessCountForPromotion = 5)
		: lru_cache_using_engine3<K,V>(function, capacity, minAccessCountForPromotion) {
	}
};

template <typename K, typename K2, typename V> class SynchronizedLRUCache2 :
		public lru_cache_using_engine3_2args<K, K2, V> {
public:
	SynchronizedLRUCache2(LRUFunction2<K, K2, V>* function, size_t capacity, int minAccessCountForPromotion = 5)
		: lru_cache_using_engine3_2args<K, K2, V>(function, capacity, minAccessCountForPromotion) {
	}
};

#endif /* SYNCHRONIZEDLRUCACHE_H_ */
