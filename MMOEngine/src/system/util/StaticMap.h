/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
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

		void setKey(const K& key) {
			StaticMapEntry::key = key;
		}

		void setValue(const V& value) {
			StaticMapEntry::value = value;
		}

		bool hasKey(const K& key) {
			return TypeInfo<K>::compare(key, StaticMapEntry::key);
		}

		friend class StaticMap<K,V>;
	};

	template<class K, class V> class StaticMap {
		static const int MAX_SIZE = 10000;

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
				assert(0 && "maximum static map size exceeded");
			}
		}

	};

  } // namespace util
} // namespace sys

using namespace sys::util;

#endif /*VECTORSET_H_*/
