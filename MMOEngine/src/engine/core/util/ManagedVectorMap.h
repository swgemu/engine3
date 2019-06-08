/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef MANAGEDVECTORMAP_H_
#define MANAGEDVECTORMAP_H_

#include "system/lang.h"

#include "engine/core/ManagedReference.h"

namespace engine {
  namespace core {
    namespace util {

	template<class K, class O> class ManagedVectorMap : public VectorMap<K, engine::core::ManagedReference<O> > {
	public:
		ManagedVectorMap() : VectorMap<K, ManagedReference<O> >() {
		}

		ManagedVectorMap(int initsize, int incr) : VectorMap<K, ManagedReference<O> >(initsize, incr) {
		}

		int put(const K& key, O* object) {
			return VectorMap<K, ManagedReference<O> >::put(key, object);
		}

		O* get(int index) {
			return VectorMap<K, ManagedReference<O> >::get(index);
		}

		O* get(const K& key) {
			return VectorMap<K, ManagedReference<O> >::get(key);
		}

		int find(const K& key) {
			return VectorMap<K, ManagedReference<O> >::find(key);
		}

		bool contains(const K& key) {
			return VectorMap<K, ManagedReference<O> >::contains(key);
		}

		bool drop(const K& key) {
			return VectorMap<K, ManagedReference<O> >::drop(key);
		}
	};

    } // namespace util
  } // namespace core
} // namespace engine

using namespace engine::core::util;

#endif /* MANAGEDVECTORMAP_H_ */
