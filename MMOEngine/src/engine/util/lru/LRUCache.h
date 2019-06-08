/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * LRUCache.h
 *
 *  Created on: 08/08/2015
 *      Author: victor
 */

#ifndef LRUCACHE_H_
#define LRUCACHE_H_

#include "lru_cache.h"

#include <map>

#ifdef CXX11_COMPILER
#include <unordered_map>
#endif

#ifdef CXX11_COMPILER
template <typename K,typename V> struct LRUCache {
	typedef lru_cache_using_std<K,V,std::unordered_map> type;
};
#else
template <typename K,typename V> class LRUCache :
	public lru_cache_using_std<K,V,std::map> {
public:
	  LRUCache(V (*f)(const K&),
	    size_t c) : lru_cache_using_std<K,V,std::map>(f, c) {
	  }
};
#endif


#endif /* LRUCACHE_H_ */
