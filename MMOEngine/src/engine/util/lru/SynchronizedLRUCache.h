/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
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

template <typename KeyType, typename Arg1, typename Arg2, typename V> class SynchronizedLRUCache2 :
		public lru_cache_using_engine3_2args<KeyType, Arg1, Arg2, V> {
public:
	SynchronizedLRUCache2(LRUFunction2<KeyType, Arg1, Arg2, V>* function, size_t capacity, int minAccessCountForPromotion = 5)
		: lru_cache_using_engine3_2args<KeyType, Arg1, Arg2, V>(function, capacity, minAccessCountForPromotion) {
	}
};

#endif /* SYNCHRONIZEDLRUCACHE_H_ */
