/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef STATICMAP_H_
#define STATICMAP_H_

#include "system/platform.h"

namespace sys {
  namespace util {

	template<class K, class V> class StaticMap;

	template<class K, class V> class StaticMapEntry : public Variable {
		K key;
		V value;

	public:
		StaticMapEntry(K key, V value) {
			StaticMapEntry::key = key;
			StaticMapEntry::value = value;
		}

		int compareTo(const StaticMapEntry& e) const {
			return TypeInfo<K>::compare(key, e.key);
		}

		K& getKey() {
			return key;
		}

		V& getValue() {
			return value;
		}

		const K& getKey() const {
			return key;
		}

		const V& getValue() const {
			return value;
		}

		void setKey(const K& key) {
			StaticMapEntry::key = key;
		}

		void setValue(const V& value) {
			StaticMapEntry::value = value;
		}

		bool hasKey(const K& key) const {
			return TypeInfo<K>::compare(key, StaticMapEntry::key);
		}

		friend class StaticMap<K,V>;
	};

	template<class K, class V> class StaticMap {
		constexpr static const int MAX_SIZE = 10000;

		StaticMapEntry<K, V> entries[MAX_SIZE];

		int count;

	public:
		StaticMap() {
			count = 0;
		}

		void put(const K& key, const V& value) {
			for(unsigned int i = 0; i < count; i++) {
				StaticMapEntry<K, V>& entry = entries[i];

				if (entry.hasKey(key)) {
					 entry.setValue(value);

					 return;
				}
			}

			if (count < MAX_SIZE) {
				StaticMapEntry<K, V>& entry = entries[count];
				entry.setKey(key);
				entry.setValue(value);

				count++;
			} else {
				E3_ABORT("maximum static map size exceeded");
			}
		}

	};

  } // namespace util
} // namespace sys

using namespace sys::util;

#endif /*VECTORSET_H_*/
